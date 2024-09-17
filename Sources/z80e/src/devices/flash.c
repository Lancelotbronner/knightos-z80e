#include <z80e/devices/flash.h>

#include <z80e/ti/asic.h>

//MARK: - Flash Status

static unsigned char __flash_mirror_read(device_t device) {
	asic_t asic = device->data;
	return device_read(asic_device(asic, 0x04));
}

//MARK: - Flash Control

static void __flash_control_write(device_t device, unsigned char value) {
	asic_t asic = device->data;
	// TODO: check permissions
	asic->mmu.flash_unlocked = value & 1;
}

void port_flash_control(device_t device, asic_t asic) {
	device->data = asic;
	device->read = __flash_mirror_read;
	device->write = __flash_control_write;

	if (asic->device == TI83p)
		port_null(device, true, false);
}

//MARK: - Flash Exclusion

static void __flash_exclusion_write(device_t device, unsigned char value) {
	// TODO: Implement flash exclusion writes
}

void port_flash_exclusion(device_t device, asic_t asic) {
	device->data = asic;
	device->read = __flash_mirror_read;
	device->write = __flash_exclusion_write;

	if (asic->device == TI83p)
		port_null(device, true, false);
}

//MARK: - Flash Size

static unsigned char __flash_size_read(device_t device) {
	return device->data;
}

static void __flash_size_write(device_t device, unsigned char value) {
	device->data = value & 0x33;
}

void port_flash_size(device_t device) {
	device->data = 0x33;
	device->read = __flash_size_read;
	device->write = __flash_size_write;
}
