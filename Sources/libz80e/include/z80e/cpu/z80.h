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

typedef unsigned char (*memory16_read8_t)(void * _Null_unspecified data, uint16_t address);
typedef void (*memory16_write8_t)(void * _Null_unspecified data, uint16_t address, unsigned char value);

struct z80_cpu {
	struct device devices[0x100];
	struct z80_state registers;
	union {
		struct {
			uint8_t : 2;
			uint8_t int_mode : 2;
		};
		struct {
			bool IFF1 : 1;
			bool IFF2 : 1;
			bool : 1;
			bool : 1;
			// Internal use:
			bool IFF_wait : 1;
			bool halted : 1;
			bool interrupt : 1;
		};
	};
	uint8_t bus;
	uint16_t prefix;
	//TODO: Abstract into a `struct mmu16` which manages a list of `struct memory16`?
	void * _Nullable memory;
	_Nullable memory16_read8_t memory_read;
	_Nullable memory16_write8_t memory_write;

	struct {
		struct hooks_register register_read;
		struct hooks_register register_write;
		struct hooks_port port_in;
		struct hooks_port port_out;
	} hook;
};

int parity(uint8_t x);
void print_state(const z80_cpu_t _Nonnull);

uint8_t cpu_read_register_byte(const z80_cpu_t _Nonnull, enum z80_registers);
uint16_t cpu_read_register_word(const z80_cpu_t _Nonnull, enum z80_registers);

uint8_t cpu_write_register_byte(const z80_cpu_t _Nonnull, enum z80_registers, uint8_t);
uint16_t cpu_write_register_word(const z80_cpu_t _Nonnull, enum z80_registers, uint16_t);

device_t _Nonnull cpu_device(const z80_cpu_t _Nonnull cpu, unsigned char i);
uint8_t cpu_read_byte(const z80_cpu_t _Nonnull cpu, uint16_t address);
void cpu_write_byte(const z80_cpu_t _Nonnull cpu, uint16_t address, uint8_t value);
uint16_t cpu_read_word(const z80_cpu_t _Nonnull cpu, uint16_t address);
void cpu_write_word(const z80_cpu_t _Nonnull cpu, uint16_t address, uint16_t value);
int cpu_execute(const z80_cpu_t _Nonnull cpu, int cycles);
