//
//  cpu/z80.h
//  z80e
//
//  Created by Christophe Bronner on 2024-03-08.
//

#pragma once

#include <z80e/device.h>
#include <z80e/types.h>
#include <z80e/cpu/z80_types.h>
#include <z80e/debugging/hooks.h>

#include <stdint.h>

struct z80_cpu {
	struct device devices[0x100];
	struct z80_state registers;
	struct {
		uint8_t IFF1 : 1;
		uint8_t IFF2 : 1;
		uint8_t int_mode : 2;
		// Internal use:
		uint8_t IFF_wait : 1;
		uint8_t halted : 1;
	};
	uint8_t bus;
	uint16_t prefix;
	void * _Nullable memory;
	uint8_t (* _Nullable read_byte)(void * _Null_unspecified, uint16_t);
	void (* _Nullable write_byte)(void * _Null_unspecified, uint16_t, uint8_t);
	bool interrupt;

	struct {
		struct hooks_register register_read;
		struct hooks_register register_write;
		struct hooks_port port_in;
		struct hooks_port port_out;
	} hook;
};

int parity(uint8_t x);
void print_state(z80_cpu_t _Nonnull);

uint8_t cpu_read_register_byte(z80_cpu_t _Nonnull, enum z80_registers);
uint16_t cpu_read_register_word(z80_cpu_t _Nonnull, enum z80_registers);

uint8_t cpu_write_register_byte(z80_cpu_t _Nonnull, enum z80_registers, uint8_t);
uint16_t cpu_write_register_word(z80_cpu_t _Nonnull, enum z80_registers, uint16_t);

device_t _Nonnull cpu_device( z80_cpu_t _Nonnull cpu, unsigned char i);
uint8_t cpu_read_byte(z80_cpu_t _Nonnull cpu, uint16_t address);
void cpu_write_byte(z80_cpu_t _Nonnull cpu, uint16_t address, uint8_t value);
uint16_t cpu_read_word(z80_cpu_t _Nonnull cpu, uint16_t address);
void cpu_write_word(z80_cpu_t _Nonnull cpu, uint16_t address, uint16_t value);
int cpu_execute(z80_cpu_t _Nonnull cpu, int cycles);
