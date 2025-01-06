#include <z80e/peripherals/flash.h>

#include <z80e/ti/asic.h>

//MARK: - Flash Status

static unsigned char __flash_mirror_read(peripheral_t peripheral) {
	asic_t asic = peripheral->data;
	return peripheral_read(asic_device(asic, 0x04));
}

//MARK: - Flash Control

static void __flash_control_write(peripheral_t peripheral, unsigned char value) {
	asic_t asic = peripheral->data;
	// TODO: check permissions
	asic->mmu.flash_unlocked = value;
}

void port_flash_control(peripheral_t peripheral, asic_t asic) {
	peripheral->data = asic;
	peripheral->read = __flash_mirror_read;
	peripheral->write = __flash_control_write;

	if (asic->peripheral == TI83p)
		port_null(peripheral, true, false);
}

//MARK: - Flash Exclusion

static void __flash_exclusion_write(peripheral_t peripheral, unsigned char value) {
	// TODO: Implement flash exclusion writes
}

void port_flash_exclusion(peripheral_t peripheral, asic_t asic) {
	peripheral->data = asic;
	peripheral->read = __flash_mirror_read;
	peripheral->write = __flash_exclusion_write;

	if (asic->peripheral == TI83p)
		port_null(peripheral, true, false);
}

//MARK: - Flash Size

static unsigned char __flash_size_read(peripheral_t peripheral) {
	return peripheral->data;
}

static void __flash_size_write(peripheral_t peripheral, unsigned char value) {
	peripheral->data = value & 0x33;
}

void port_flash_size(peripheral_t peripheral) {
	peripheral->data = 0x33;
	peripheral->read = __flash_size_read;
	peripheral->write = __flash_size_write;
}
