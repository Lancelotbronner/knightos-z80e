#include <z80e/devices/mapping.h>
#include <z80e/log.h>
#include <z80e/ti/asic.h>

void mapping_init(mapping_device_t mapping, asic_t asic) {
	*mapping = (struct mapping_device){
		.asic = asic,
		.flashA = true,
		.flashB = true,
	};
	mapping_reload(mapping);
}

void mapping_reload(mapping_device_t mapping) {
	ti_mmu_t mmu = &mapping->asic->mmu;

	mmu_configure(mmu, 0, 0, true);

	if (mapping->mode) {
		uint8_t page2 = mapping->a;
		if (mapping->asic->device == TI83p)
			page2 |= 1;

		mmu_configure(mmu, 1, mapping->a & 0xFE, mapping->flashA);
		mmu_configure(mmu, 2, page2, mapping->flashA);
		mmu_configure(mmu, 3, mapping->b, mapping->flashB);
	} else {
		uint8_t page3 = 0;
		if (mapping->asic->device != TI83p)
			page3 = mapping->page;

		mmu_configure(mmu, 1, mapping->a, mapping->flashA);
		mmu_configure(mmu, 2, mapping->b, mapping->flashB);
		mmu_configure(mmu, 3, page3, false);
	}

	for (int i = 0; i < 4; i++) {
		mmu_bank_t bank;
		if (mmu_validate(mmu, i, &bank))
			continue;

		z80e_error("memorymapping", "%s page 0x%02X doesn't exist! (at 0x%04X)", bank.flash ? "flash" : "ram", bank.page, mapping->asic->cpu.registers.PC);
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

	z80e_debug("memorymapping", "set mapping mode to %d (at 0x%04X)", mapping->mode, mapping->asic->cpu.registers.PC);
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

void port_mapping_status(device_t device, mapping_device_t mapping) {
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
	z80e_debug("memorymapping", "Set ram banking page to %d (at 0x%04X)", mapping->page, mapping->asic->cpu.registers.PC);
	mapping_reload(mapping);
}

void port_mapping_paging(device_t device, mapping_device_t mapping) {
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

	z80e_debug("memorymapping", "Set bank A page to %c:%02X (at 0x%04X)", mapping->flashA ? 'F' : 'R',  mapping->a, mapping->asic->cpu.registers.PC);
	mapping_reload(mapping);
}

void port_mapping_bankA(device_t device, mapping_device_t mapping) {
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

	z80e_debug("memorymapping", "Set bank B page to %c:%02X (at 0x%04X)", mapping->flashB ? 'F' : 'R',  mapping->b, mapping->asic->cpu.registers.PC);
	mapping_reload(mapping);
}

void port_mapping_bankB(device_t device, mapping_device_t mapping) {
	device->data = mapping;
	device->read = __mapping_bankB_read;
	device->write = __mapping_bankB_write;
}
