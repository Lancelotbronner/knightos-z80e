#include <z80e/devices/flash.h>

#include <z80e/ti/asic.h>
#include <z80e/log/log.h>
#include <z80e/ti/memory.h>

#include <stdlib.h>
#include <string.h>

//MARK: - Device Management

static unsigned char __flash_mirror_read(device_t device) {
	asic_t *asic = device->data;
	device_t port4 = &asic->cpu.devices[0x04];
	if (port4->read)
		return port4->read(device);
	return 0x00;
}

static void __flash_control_write(device_t device, unsigned char value) {
	asic_t *asic = device->data;
	// TODO: check permissions
	asic->mmu.flash_unlocked = value & 1;
}

static void __flash_exclusion_write(device_t device, unsigned char value) {
	// TODO: Implement flash exclusion writes
}

static unsigned char __flash_size_read(device_t device) {
	return device->value;
}

static void __flash_size_write(device_t device, unsigned char value) {
	device->value = value & 0x33;
}

void device_flash_control(device_t device, asic_t *asic) {
	device->data = asic;
	device->read = __flash_mirror_read;
	device->write = __flash_control_write;

	if (asic->device == TI83p)
		device_null(device, true, false);
}

void device_flash_exclusion(device_t device, asic_t *asic) {
	device->data = asic;
	device->read = __flash_mirror_read;
	device->write = __flash_exclusion_write;

	if (asic->device == TI83p)
		device_null(device, true, false);
}

void device_flash_size(device_t device) {
	device->value = 0x33;
	device->read = __flash_size_read;
	device->write = __flash_size_write;
}
