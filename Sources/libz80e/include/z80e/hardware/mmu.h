//
//  hardware/mmu.h
//  z80e
//
//  Created by Christophe Bronner on 2024-09-17.
//

#pragma once

#include <z80e/debugging/hooks.h>
#include <z80e/ti/ti.h>
#include <z80e/types.h>

#include <stdint.h>

typedef struct {
	uint8_t page;
	bool flash : 1;
} mmu_bank_t;

typedef struct {
	uint32_t address;
	uint32_t address_mask;
	uint8_t value;
	uint8_t value_mask;
} flash_write_t;

struct ti_mmu {
	uint8_t *ram;
	uint8_t *flash;

	struct {
		uint8_t page[4];
		union {
			uint8_t flash;
			struct {
				bool flash0 : 1;
				bool flash1 : 1;
				bool flash2 : 1;
				bool flash3 : 1;
			};
		};
	} banks;

	struct {
		uint16_t ram_pages;
		uint16_t flash_pages;
	} settings;

	flash_write_t flash_writes[6];

	struct {
		struct hooks_memory memory_read;
		struct hooks_memory memory_write;
	} hook;

	int flash_write_index;
	bool flash_unlocked : 1;
};

void mmu_init(ti_mmu_t mmu, ti_device_type device_type);
void mmu_deinit(ti_mmu_t mmu);

mmu_bank_t mmu_bank(ti_mmu_t const mmu, unsigned int bank);

void mmu_configure(ti_mmu_t mmu, unsigned int bank, uint8_t page, bool flash);
void mmu_page(ti_mmu_t mmu, unsigned int bank, uint8_t page);
void mmu_flash(ti_mmu_t mmu, unsigned int bank, bool flash);

bool mmu_validate(ti_mmu_t mmu, unsigned int bank, mmu_bank_t *info);

unsigned char mmu_read(ti_mmu_t mmu, uint16_t address);
void mmu_write(ti_mmu_t mmu, uint16_t address, uint8_t value);
void mmu_force_write(ti_mmu_t mmu, uint16_t address, uint8_t value);

/// Calculates the address relative to the specified bank.
/// - Parameters:
///   - mmu: The MMU to interface with.
///   - bank: The bank in which to select the address
///   - address: The address to remap into a bank
uint32_t mmu_address(ti_mmu_t const mmu, mmu_bank_t bank, uint16_t address);

/// Provides an *unsafe* pointer to the physical memory represented by the provided address.
/// - Parameters:
///   - mmu: The MMU to interface with.
///   - address: The address to point to.
uint8_t *mmu_destination(ti_mmu_t const mmu, uint16_t address);
