//
//  device.c
//  z80e
//
//  Created by Christophe Bronner on 2024-03-18.
//

#include <z80e/device.h>

#include <assert.h>
#include <stdint.h>

unsigned char device_read(struct z80_device device) {
	if (device.read)
		return device.read(device.data);
	return 0x00;
}

void device_write(struct z80_device device, unsigned char value) {
	if (device.write)
		device.write(device.data, value);
}

//MARK: - Mirror Device

static uint8_t __mirror_read(device_t device) {
	device_read(*device);
}

static void __mirror_write(device_t device, uint8_t value) {
	device_write(*device, value);
}

void device_mirror(device_t device, const device_t other) {
	assert(device && other);
	device->data = other;
	device->read = (read_t)__mirror_read;
	device->write = (write_t)__mirror_write;
}

//MARK: - Unimplemented Device

//TODO: Once the asic is passed to device functions, use it to log in the unimplemented device
static uint8_t __unimplemented_read(void *port) {
//	log_message(asic->log, L_INFO, "asic", "Warning: attempted to read from unimplemented port 0x%02x from 0x%04X.", (int)port, d->asic->cpu.registers.PC);
	return 0x00;
}

static void __unimplemented_write(void *port, uint8_t value) {
//	log_message(asic->log, L_INFO, "asic", "Warning: attempted to write 0x%02x to unimplemented port 0x%02x from 0x%04X.", value, d->port, d->asic->cpu.registers.PC);
}

void device_unimplemented(device_t device, int port) {
	assert(device);
	device->data = (void*)(intptr_t)port;
	device->read = (read_t)__unimplemented_read;
	device->write = (write_t)__unimplemented_write;
}

//MARK: - Null Device

static uint8_t __null_read(void *) { return 0x00; }
static void __null_write(void *port, uint8_t value) { }

void device_null(device_t _Nonnull device, bool read, bool write) {
	assert(device);
	if (read)
		device->read = (read_t)__null_read;
	if (write)
		device->write = (write_t)__null_write;
}
