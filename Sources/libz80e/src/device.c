//
//  peripheral.c
//  z80e
//
//  Created by Christophe Bronner on 2024-03-18.
//

#include <z80e/peripheral.h>

#include <assert.h>

unsigned char peripheral_read(const peripheral_t _Nonnull peripheral) {
	if (peripheral->read)
		return peripheral->read(peripheral);
	return 0x00;
}

void peripheral_write(const peripheral_t _Nonnull peripheral, unsigned char value) {
	if (peripheral->write)
		peripheral->write(peripheral, value);
}

//MARK: - Mirror Port

static unsigned char __mirror_read(peripheral_t peripheral) {
	return peripheral_read(peripheral->data);
}

static void __mirror_write(peripheral_t peripheral, unsigned char value) {
	peripheral_write(peripheral->data, value);
}

void port_mirror(peripheral_t peripheral, const peripheral_t other) {
	assert(peripheral && other);
	peripheral->data = other;
	peripheral->read = (read_t)__mirror_read;
	peripheral->write = (write_t)__mirror_write;
}

//MARK: - Unimplemented Port

//TODO: Once the asic is passed to peripheral functions, use it to log in the unimplemented peripheral
static unsigned char __unimplemented_read(peripheral_t peripheral) {
//	z80_info("asic", "Warning: attempted to read from unimplemented port 0x%02x from 0x%04X.", (int)port, d->asic->cpu.registers.PC);
	return 0x00;
}

static void __unimplemented_write(peripheral_t peripheral, unsigned char value) {
//	z80_info("asic", "Warning: attempted to write 0x%02x to unimplemented port 0x%02x from 0x%04X.", value, d->port, d->asic->cpu.registers.PC);
}

void port_unimplemented(peripheral_t peripheral, unsigned char port) {
	assert(peripheral);
	peripheral->data = port;
	peripheral->read = __unimplemented_read;
	peripheral->write = __unimplemented_write;
}

//MARK: - Null Port

static unsigned char __null_read(peripheral_t) { return 0x00; }
static void __null_write(peripheral_t, unsigned char) { }

void port_null(peripheral_t _Nonnull peripheral, bool read, bool write) {
	assert(peripheral);
	if (read)
		peripheral->read = __null_read;
	if (write)
		peripheral->write = __null_write;
}
