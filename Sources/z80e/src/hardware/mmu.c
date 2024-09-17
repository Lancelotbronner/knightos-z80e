#include <z80e/hardware/mmu.h>
#include <z80e/cpu/z80.h>
#include <z80e/ti/ti.h>
#include <z80e/log.h>

#include <z80e/debugging/debugger.h>
#include <z80e/disassembler/disassemble.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//MARK: - Utilities

static void __mmu_page(ti_mmu_t mmu, unsigned int bank, uint8_t page) {
	mmu->banks.page[bank] = page;
}

static void __mmu_flash(ti_mmu_t mmu, unsigned int bank, bool flash) {
	if (flash)
		mmu->banks.flash |= 1 << bank;
	else
		mmu->banks.flash &= ~(1 << bank);
}

static mmu_bank_t __mmu_bank(ti_mmu_t const mmu, unsigned int bank) {
	return (mmu_bank_t){
		.page = mmu->banks.page[bank],
		.flash = !!(mmu->banks.flash & (1 << bank)),
	};
}

static uint32_t __mmu_address(ti_mmu_t const mmu, mmu_bank_t bank, uint16_t address) {
	uint32_t mapped_address = address;
	mapped_address %= 0x4000;
	mapped_address += bank.page * 0x4000;
	return mapped_address;
}

static uint8_t *__mmu_destination(ti_mmu_t const mmu, uint16_t address) {
	mmu_bank_t bank = __mmu_bank(mmu, address / 0x4000);
	uint8_t *destination = bank.flash ? mmu->flash : mmu->ram;
	return &destination[__mmu_address(mmu, bank, address)];
}

//MARK: - Bank Management

mmu_bank_t mmu_bank(ti_mmu_t const mmu, unsigned int bank) {
	if (bank < 4)
		return __mmu_bank(mmu, bank);
	return (mmu_bank_t){};
}

void mmu_configure(ti_mmu_t mmu, unsigned int bank, uint8_t page, bool flash) {
	if (bank > 3)
		return;
	__mmu_page(mmu, bank, page);
	__mmu_flash(mmu, bank, flash);
}

void mmu_page(ti_mmu_t mmu, unsigned int bank, uint8_t page) {
	if (bank > 3)
		return;
	__mmu_page(mmu, bank, page);
}

void mmu_flash(ti_mmu_t mmu, unsigned int bank, bool flash) {
	if (bank > 3)
		return;
	__mmu_flash(mmu, bank, flash);
}

bool mmu_validate(ti_mmu_t mmu, unsigned int bank, mmu_bank_t *info) {
	if (bank > 3)
		return false;

	mmu_bank_t _info = __mmu_bank(mmu, bank);
	if (info) *info = _info;

	uint16_t mask = _info.flash ? mmu->settings.flash_pages : mmu->settings.ram_pages;
	uint8_t page = _info.page & mask;
	__mmu_page(mmu, bank, page);

	return _info.page == page;
}

//MARK: - Memory Unit Management

void ti_mmu_init(ti_mmu_t mmu, ti_device_type device_type) {
	switch (device_type) {
	case TI83p:
	case TI73:
		mmu->settings.ram_pages = 3;
		mmu->settings.flash_pages = 0x20;
		break;
	case TI84p:
		mmu->settings.ram_pages = 8;
		mmu->settings.flash_pages = 0x40;
		break;
	case TI83pSE:
	case TI84pSE:
		mmu->settings.ram_pages = 8;
		mmu->settings.flash_pages = 0x80;
		break;
	case TI84pCSE:
		mmu->settings.ram_pages = 3;
		mmu->settings.flash_pages = 0x100;
		break;
	}
	mmu->ram = calloc(mmu->settings.ram_pages, 0x4000);
	mmu->flash = malloc(mmu->settings.flash_pages * 0x4000);
	memset(mmu->flash, 0xFF, mmu->settings.flash_pages * 0x4000);
	mmu->flash_unlocked = false;
	memset(mmu->flash_writes, 0, sizeof(flash_write_t) * 6);
	mmu->flash_write_index = 0;
	// Default bank mappings
	mmu->banks.flash = 0b1110;
	return mmu;
}

void ti_mmu_deinit(ti_mmu_t mmu) {
	free(mmu->ram);
	free(mmu->flash);
}

void chip_reset(ti_mmu_t mmu, uint32_t address, uint8_t value) {
	mmu->flash_write_index = 0;
}

void chip_write(ti_mmu_t mmu, uint32_t address, uint8_t value) {
	mmu->flash[address] &= value;
}

void chip_erase_sector(ti_mmu_t mmu, uint32_t address, uint8_t value) {
	uint32_t length = 0x10000;
	if ((address >> 16) + 1 == mmu->settings.flash_pages >> 2) {
		switch ((address >> 13) & 7) {
		case 0:
		case 1:
		case 2:
		case 3:
			length = 0x8000;
			break;
		case 4:
		case 5:
			length = 0x2000;
			break;
		case 6:
		case 7:
			length = 0x4000;
			break;
		}
	}
	memset(&mmu->flash[address & ~(length - 1)], 0xFF, length);
}

void chip_erase(ti_mmu_t mmu, uint32_t address, uint8_t value) {
	memset(mmu->flash, 0xFF, mmu->settings.flash_pages * 0x4000);
	z80e_warning("mmu", "Erased entire Flash chip - you probably didn't want to do that.");
}

uint8_t ti_read_byte(void *memory, uint16_t address) {
	ti_mmu_t mmu = memory;

	mmu_bank_t bank = mmu_bank(mmu, address / 0x4000);
	uint8_t mapped_address = __mmu_address(mmu, bank, address);
	uint8_t *destination = bank.flash ? mmu->flash : mmu->ram;

	if (bank.flash)
		chip_reset(mmu, mapped_address, 0);

	uint8_t value = destination[mapped_address];;
	value = hook_memory_trigger(&mmu->hook.memory_read, address, value);
	return value;
}

struct flash_pattern {
	int length;
	const flash_write_t pattern[6];
	void (*handler)(ti_mmu_t memory, uint32_t address, uint8_t value);
};

static struct flash_pattern Patterns[] = {
	{
		.length = 4,
		.pattern = {
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xA0, .value_mask = 0xFF },
			{ .address = 0x000, .address_mask = 0x000, .value = 0x00, .value_mask = 0x00 },
		},
		.handler = chip_write
	},
	{
		.length = 6,
		.pattern = {
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0x80, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0x000, .address_mask = 0x000, .value = 0x00, .value_mask = 0x00 },
		},
		.handler = chip_erase_sector
	},
	{
		.length = 6,
		.pattern = {
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0x80, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0xAA, .value_mask = 0xFF },
			{ .address = 0x555, .address_mask = 0xFFF, .value = 0x55, .value_mask = 0xFF },
			{ .address = 0xAAA, .address_mask = 0xFFF, .value = 0x10, .value_mask = 0xFF },
		},
		.handler = chip_erase
	},
	{
		.length = 0
	}
	// TODO: More patterns
};

void ti_write_byte(void *memory, uint16_t address, uint8_t value) {
	ti_mmu_t mmu = memory;

	value = hook_memory_trigger(&mmu->hook.memory_write, address, value);
	mmu_bank_t bank = mmu_bank(mmu, address / 0x4000);
	uint32_t mapped_address = __mmu_address(mmu, bank, address);

	if (!bank.flash) {
		mmu->ram[mapped_address] = value;
		return;
	}

	if (!mmu->flash_unlocked)
		return;

	flash_write_t *w = &mmu->flash_writes[mmu->flash_write_index++];
	w->address = address;
	w->value = value;

	bool partial_match = false;
	struct flash_pattern *pattern;

	//TODO: what?
	for (pattern = Patterns; pattern->length; pattern++) {
		int i = 0;
		while (i < mmu->flash_write_index && i < pattern->length &&
			   (mmu->flash_writes[i].address & pattern->pattern[i].address_mask) == pattern->pattern[i].address &&
			   (mmu->flash_writes[i].value & pattern->pattern[i].value_mask) == pattern->pattern[i].value)
			i++;

		if (i == pattern->length) {
			pattern->handler(mmu, mapped_address, value);
			partial_match = false;
			break;
		} else if (i == mmu->flash_write_index) {
			partial_match = true;
		}
	}

	if (!partial_match)
		chip_reset(mmu, mapped_address, value);
}

void mmu_force_write(void *memory, uint16_t address, uint8_t value) {
	ti_mmu_t mmu = memory;
	value = hook_memory_trigger(&mmu->hook.memory_write, address, value);
	__mmu_destination(mmu, address)[0] = value;
}
