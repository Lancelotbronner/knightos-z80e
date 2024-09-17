#pragma once

#include <z80e/types.h>
#include <z80e/cpu/z80_types.h>

#include <stdint.h>

// Note: All `hook_<domain>` structs have the following assumptions:
// - callback is the first field
// - Their size is at most 3 `void*`

typedef struct hook {
	void *list;
	void *callback;
} hook_t;

void hook_cancel(hook_t hook);

//MARK: - Memory Hooks

typedef struct hook_memory *hook_memory_t;
typedef struct hooks_memory *hooks_memory_t;
typedef uint8_t (*hook_memory_callback_t)(void *data, uint16_t address, uint8_t value);

struct hook_memory {
	hook_memory_callback_t callback;
	void *data;
	uint16_t range_start;
	uint16_t range_end;
};

struct hooks_memory {
	int count;
	int capacity;
	struct hook_memory *storage;
};

hook_t hook_memory_insert(hooks_memory_t hooks, hook_memory_t const hook);
hook_t hook_memory_emplace(hooks_memory_t hooks, uint16_t address_start, uint16_t address_end, void *data, hook_memory_callback_t callback);

uint8_t hook_memory_trigger(hooks_memory_t hooks, uint16_t address, uint8_t value);

//MARK: - Register Hooks

typedef struct hook_register *hook_register_t;
typedef struct hooks_register *hooks_register_t;
typedef uint16_t (*hook_register_callback_t)(void *data, enum z80_registers reg, uint16_t value);

struct hook_register {
	hook_register_callback_t callback;
	void *data;
	enum z80_registers registers;
};

struct hooks_register {
	int count;
	int capacity;
	struct hook_register *storage;
};

hook_t hook_register(hooks_register_t hooks, hook_register_t const hook);
hook_t hook_register_emplace(hooks_register_t hooks, enum z80_registers registers, void *data, hook_register_callback_t callback);

uint8_t hook_register_trigger(hooks_register_t hooks, enum z80_registers registers, uint8_t value);

//MARK: - Port Hooks

typedef struct hook_port *hook_port_t;
typedef struct hooks_port *hooks_port_t;
typedef uint8_t (*hook_port_callback_t)(void *data, uint8_t port, uint8_t value);

struct hook_port {
	hook_port_callback_t callback;
	void *data;
	uint8_t range_start;
	uint8_t range_end;
};

struct hooks_port {
	int count;
	int capacity;
	struct hook_port *storage;
};

hook_t hook_port(hooks_port_t hooks, hook_port_t const hook);
hook_t hook_port_emplace(hooks_port_t hooks, uint8_t range_start, uint8_t range_end, void *data, hook_port_callback_t callback);

uint8_t hook_port_trigger(hooks_port_t hooks, uint8_t port, uint8_t value);

//MARK: - Execution Hooks

typedef struct hook_execution *hook_execution_t;
typedef struct hooks_execution *hooks_execution_t;
typedef void (*hook_execution_callback_t)(void *data, uint16_t address);

struct hook_execution {
	hook_execution_callback_t callback;
	void *data;
};

struct hooks_execution {
	int count;
	int capacity;
	struct hook_execution *storage;
};

hook_t hook_execution(hooks_execution_t hooks, hook_execution_t const hook);
hook_t hook_execution_emplace(hooks_execution_t hooks, void *data, hook_execution_callback_t callback);

void hook_execution_trigger(hooks_execution_t hooks, uint16_t address);

//MARK: - LCD Hooks

typedef struct hook_lcd *hook_lcd_t;
typedef struct hooks_lcd *hooks_lcd_t;
typedef void (*hook_lcd_callback_t)(void *data, ti_bw_lcd_t *lcd);

struct hook_lcd {
	hook_lcd_callback_t callback;
	void *data;
};

struct hooks_lcd {
	int count;
	int capacity;
	struct hook_lcd *storage;
};

hook_t hook_lcd(hooks_lcd_t hooks, hook_lcd_t const hook);
hook_t hook_lcd_emplace(hooks_lcd_t hooks, void *data, hook_lcd_callback_t callback);

void hook_lcd_trigger(hooks_lcd_t hooks, ti_bw_lcd_t *lcd);
