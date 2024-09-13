#include <z80e/devices/mapping.h>

#include <z80e/ti/asic.h>

void mapping_init(mapping_device_t mapping, asic_t asic) {
	*mapping = (struct mapping_device){
		.asic = asic,
		.flashA = 1,
		.flashB = 1,
	};
	mapping_reload(mapping);
}

void mapping_reload(mapping_device_t mapping) {
	ti_mmu_bank_state_t *banks = mapping->asic->mmu.banks;

	banks[0].page = 0;
	banks[0].flash = 1;

	if (mapping->mode) {
		banks[1].page = mapping->a & 0xFE;
		banks[1].flash = mapping->flashA;
		if (mapping->asic->device == TI83p) {
			banks[2].page = mapping->a;
			banks[2].flash = mapping->flashA;
		} else {
			banks[2].page = mapping->a | 1;
			banks[2].flash = mapping->flashA;
		}
		banks[3].page = mapping->b;
		banks[3].flash = mapping->flashB;
	} else {
		banks[1].page = mapping->a;
		banks[1].flash = mapping->flashA;
		banks[2].page = mapping->b;
		banks[2].flash = mapping->flashB;
		if (mapping->asic->device == TI83p) {
			banks[3].page = 0;
			banks[3].flash = 0;
		} else {
			banks[3].page = mapping->page;
			banks[3].flash = 0;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (banks[i].flash && banks[i].page > mapping->asic->mmu.settings.flash_pages) {
			z80_error("memorymapping", "ERROR: Flash page 0x%02X doesn't exist! (at 0x%04X)", banks[i].page, mapping->asic->cpu.registers.PC);
			banks[i].page &= mapping->asic->mmu.settings.flash_pages;
		} else if (!banks[i].flash && banks[i].page > mapping->asic->mmu.settings.ram_pages) {
			z80_error("memorymapping", "ERROR: RAM page 0x%02X doesn't exist! (at 0x%04X)", banks[i].page, mapping->asic->cpu.registers.PC);
			banks[i].page &= mapping->asic->mmu.settings.ram_pages;
		}
	}
}

//MARK: - Mapping Status

static unsigned char __mapping_status_read(device_t device) {
	mapping_device_t mapping = device->data;
	return mapping->asic->interrupts.interrupted.flags;
}

static void __mapping_status_write(device_t device, unsigned char value) {
	mapping_device_t mapping = device->data;
	mapping->mode = value;

	z80_debug("memorymapping", "Set mapping mode to %d (at 0x%04X)", mapping->mode, mapping->asic->cpu.registers.PC);
	mapping_reload(mapping);

	static double timer1[2][4] = {
		{ 560, 248, 170, 118 },
		{ 512, 227.55, 146.29, 107.79 }
	};
	static double timer2[2][4] = {
		{ 1120, 497, 344, 236 },
		{ 1024, 455.11, 292.57, 215.28 }
	};

	asic_t asic = mapping->asic;
	bool isNotTi83p = asic->device != TI83p;
	value >>= 1;
	uint8_t speed = value & 3;
	asic_timer1_frequency(asic, timer1[isNotTi83p][speed]);
	asic_timer2_frequency(asic, timer2[isNotTi83p][speed]);
}

void device_mapping_status(device_t device, mapping_device_t mapping) {
	device->data = mapping;
	device->read = __mapping_status_read;
	device->write = __mapping_status_write;
}

//MARK: - Mapping Paging

static unsigned char __mapping_paging_read(device_t device) {
	mapping_device_t mapping = device->data;
	return mapping->page;
}

static void __mapping_paging_write(device_t device, unsigned char value) {
	mapping_device_t mapping = device->data;
	mapping->page = value;
	z80_debug("memorymapping", "Set ram banking page to %d (at 0x%04X)", mapping->page, mapping->asic->cpu.registers.PC);
	mapping_reload(mapping);
}

void device_mapping_paging(device_t device, mapping_device_t mapping) {
	device->data = mapping;
	device->read = __mapping_paging_read;
	device->write = __mapping_paging_write;
}

//MARK: - Mapping Bank A

static unsigned char __mapping_bankA_read(device_t device) {
	mapping_device_t mapping = device->data;
	unsigned char value = mapping->a;
	if (!mapping->flashA)
		value |= mapping->asic->device == TI83p ? 0x40 : 0x80;
	return value;
}

static void __mapping_bankA_write(device_t device, unsigned char value) {
	mapping_device_t mapping = device->data;
	bool is_flash = !(value & (mapping->asic->device == TI83p ? 0x40 : 0x80));

	if (mapping->asic->device == TI83p) {
		value &= 0x1F;
	} else {
		value &= 0x7F;
	}

	mapping->flashA = is_flash;
	mapping->a = value;

	z80_debug("memorymapping", "Set bank A page to %c:%02X (at 0x%04X)", mapping->flashA ? 'F' : 'R',  mapping->a, mapping->asic->cpu.registers.PC);
	mapping_reload(mapping);
}

void device_mapping_bankA(device_t device, mapping_device_t mapping) {
	device->data = mapping;
	device->read = __mapping_bankA_read;
	device->write = __mapping_bankA_write;
}

//MARK: - Mapping Bank B

static unsigned char __mapping_bankB_read(device_t device) {
	mapping_device_t mapping = device->data;
	unsigned char value = mapping->b;
	if (!mapping->flashB)
		value |= mapping->asic->device == TI83p ? 0x40 : 0x80;
	return value;
}

static void __mapping_bankB_write(device_t device, unsigned char value) {
	mapping_device_t mapping = device->data;
	bool is_flash = !(value & (mapping->asic->device == TI83p ? 0x40 : 0x80));

	if (mapping->asic->device == TI83p) {
		value &= 0x1F;
	} else {
		value &= 0x7F;
	}

	mapping->flashB = is_flash;
	mapping->b = value;

	z80_debug("memorymapping", "Set bank B page to %c:%02X (at 0x%04X)", mapping->flashB ? 'F' : 'R',  mapping->b, mapping->asic->cpu.registers.PC);
	mapping_reload(mapping);
}

void device_mapping_bankB(device_t device, mapping_device_t mapping) {
	device->data = mapping;
	device->read = __mapping_bankB_read;
	device->write = __mapping_bankB_write;
}
