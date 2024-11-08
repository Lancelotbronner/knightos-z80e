/*
 * Implements the 83+SE/84+((C)SE) crystal timers
 */

#include <z80e/devices/crystal.h>

#include <z80e/device.h>

//MARK: - Crystal Frequency

static unsigned char __crystal_read(device_t device) {
	unsigned char frequency = device->data;
	return frequency;
}

static void __crystal_write(device_t device, unsigned char value) {
	device->data = value;
}

void port_crystal_frequency(device_t device) {
	device->data = 0;
	device->read = __crystal_read;
	device->write = __crystal_write;
}

void port_crystal_loop(device_t device) {
	device->data = 0;
	device->read = __crystal_read;
	device->write = __crystal_write;
}

void port_crystal_count(device_t device) {
	device->data = 0;
	device->read = __crystal_read;
	device->write = __crystal_write;
}
