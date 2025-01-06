/*
 * Implements the 83+SE/84+((C)SE) crystal timers
 */

#include <z80e/peripherals/crystal.h>

#include <z80e/peripheral.h>

//MARK: - Crystal Frequency

static unsigned char __crystal_read(peripheral_t peripheral) {
	unsigned char frequency = peripheral->data;
	return frequency;
}

static void __crystal_write(peripheral_t peripheral, unsigned char value) {
	peripheral->data = value;
}

void port_crystal_frequency(peripheral_t peripheral) {
	peripheral->data = 0;
	peripheral->read = __crystal_read;
	peripheral->write = __crystal_write;
}

void port_crystal_loop(peripheral_t peripheral) {
	peripheral->data = 0;
	peripheral->read = __crystal_read;
	peripheral->write = __crystal_write;
}

void port_crystal_count(peripheral_t peripheral) {
	peripheral->data = 0;
	peripheral->read = __crystal_read;
	peripheral->write = __crystal_write;
}
