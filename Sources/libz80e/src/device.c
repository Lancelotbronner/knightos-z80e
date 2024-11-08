//
//  device.c
//  z80e
//
//  Created by Christophe Bronner on 2024-03-18.
//

#include <z80e/device.h>

#include <assert.h>

unsigned char device_read(const device_t _Nonnull device) {
	if (device->read)
		return device->read(device);
	return 0x00;
}

void device_write(const device_t _Nonnull device, unsigned char value) {
	if (device->write)
		device->write(device, value);
}

//MARK: - Mirror Port

static unsigned char __mirror_read(device_t device) {
	return device_read(device->data);
}

static void __mirror_write(device_t device, unsigned char value) {
	device_write(device->data, value);
}

void port_mirror(device_t device, const device_t other) {
	assert(device && other);
	device->data = other;
	device->read = (read_t)__mirror_read;
	device->write = (write_t)__mirror_write;
}

//MARK: - Unimplemented Port

//TODO: Once the asic is passed to device functions, use it to log in the unimplemented device
static unsigned char __unimplemented_read(device_t device) {
//	z80_info("asic", "Warning: attempted to read from unimplemented port 0x%02x from 0x%04X.", (int)port, d->asic->cpu.registers.PC);
	return 0x00;
}

static void __unimplemented_write(device_t device, unsigned char value) {
//	z80_info("asic", "Warning: attempted to write 0x%02x to unimplemented port 0x%02x from 0x%04X.", value, d->port, d->asic->cpu.registers.PC);
}

void port_unimplemented(device_t device, unsigned char port) {
	assert(device);
	device->data = port;
	device->read = __unimplemented_read;
	device->write = __unimplemented_write;
}

//MARK: - Null Port

static unsigned char __null_read(device_t) { return 0x00; }
static void __null_write(device_t, unsigned char) { }

void port_null(device_t _Nonnull device, bool read, bool write) {
	assert(device);
	if (read)
		device->read = __null_read;
	if (write)
		device->write = __null_write;
}
