#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <z80e/debugger/hooks.h>
#include <z80e/cpu/z80.h>
#include <z80e/ti/asic.h>
#include <z80e/ti/memory.h>

//MARK: - Lifecycle

size_t sizeof_hook() {
	return sizeof(struct hook_info);
}

void hook_init(hook_info_t info, asic_t asic) {
	*info = (struct hook_info){};
	asic->cpu.hook = info;
	asic->mmu.hook = info;
	return info;
}

//MARK: - Hooks

typedef struct hook_list *hook_list_t;

struct hook_callback {
	void *data[3];
};

struct hook_list {
	int count;
	int capacity;
	struct hook_callback *storage;
};

static void hooks_resize(hook_list_t list) {
	void *storage = malloc(list->capacity * sizeof(struct hook_callback));
	memcpy(storage, list->storage, list->count * sizeof(struct hook_callback));
	list->storage = storage;
}

static hook_t hooks_insert(hook_list_t list, struct hook_callback *callback) {
	if (!callback || !callback->data[0])
		return;

	if (list->count >= list->capacity) {
		list->capacity += 8;
		hooks_resize(list);
	}

	list->storage[list->count++] = *callback;

	return (struct hook){
		.list = list,
		.callback = callback,
	};
}

void hook_cancel(hook_t hook) {
	hook_list_t list = hook.list;
	for (int i = 0; i < list->count; i++) {
		if (list->storage[i].data[0] != hook.callback)
			continue;
		list->storage[i] = list->storage[--list->count];
		return;
	}
}

//MARK: - Memory Hooks

hook_t hook_memory(hooks_memory_t hooks, hook_memory_t const hook) {
	return hooks_insert((hook_list_t)hooks, (struct hook_callback *)hook);
}

hook_t hook_memory_emplace(hooks_memory_t hooks, uint16_t address_start, uint16_t address_end, void *data, hook_memory_callback_t callback) {
	struct hook_memory hook = (struct hook_memory){
		.data = data,
		.callback = callback,
		.range_start = address_start,
		.range_end = address_end,
	};
	return hook_memory(hooks, &hook);
}

uint8_t hook_memory_trigger(hooks_memory_t hooks, uint16_t address, uint8_t value) {
	for (int i = 0; i < hooks->count; i++) {
		hook_memory_t hook = &hooks->storage[i];
		if (address >= hook->range_start && address <= hook->range_end)
			hook->callback(hook->data, address, value);
	}
	return value;
}

//MARK: - Register Hooks

hook_t hook_register(hooks_register_t hooks, hook_register_t const hook) {
	return hooks_insert((hook_list_t)hooks, (struct hook_callback *)hook);
}

hook_t hook_register_emplace(hooks_register_t hooks, enum z80_registers registers, void *data, hook_register_callback_t callback) {
	struct hook_register hook = (struct hook_register){
		.data = data,
		.callback = callback,
		.registers = registers,
	};
	return hook_register(hooks, &hook);
}

uint8_t hook_register_trigger(hooks_register_t hooks, enum z80_registers registers, uint8_t value) {
	for (int i = 0; i < hooks->count; i++) {
		hook_register_t hook = &hooks->storage[i];
		if (hook->registers & registers)
			hook->callback(hook->data, registers, value);
	}
	return value;
}

//MARK: - Port Hooks

hook_t hook_port(hooks_port_t hooks, hook_port_t const hook) {
	return hooks_insert((hook_list_t)hooks, (struct hook_callback *)hook);
}

hook_t hook_port_emplace(hooks_port_t hooks, uint8_t range_start, uint8_t range_end, void *data, hook_port_callback_t callback) {
	struct hook_port hook = (struct hook_port){
		.data = data,
		.callback = callback,
		.range_start = range_start,
		.range_end = range_end,
	};
	return hook_port(hooks, &hook);
}

uint8_t hook_port_trigger(hooks_port_t hooks, uint8_t port, uint8_t value) {
	for (int i = 0; i < hooks->count; i++) {
		hook_port_t hook = &hooks->storage[i];
		if (port >= hook->range_start && port <= hook->range_end)
			hook->callback(hook->data, port, value);
	}
	return value;
}

//MARK: - Execution Hooks

hook_t hook_execution(hooks_execution_t hooks, hook_execution_t const hook) {
	return hooks_insert((hook_list_t)hooks, (struct hook_callback *)hook);
}

hook_t hook_execution_emplace(hooks_execution_t hooks, void *data, hook_execution_callback_t callback) {
	struct hook_execution hook = (struct hook_execution){
		.data = data,
		.callback = callback,
	};
	return hook_execution(hooks, &hook);
}

void hook_execution_trigger(hooks_execution_t hooks, uint16_t address) {
	for (int i = 0; i < hooks->count; i++) {
		hook_execution_t hook = &hooks->storage[i];
		hook->callback(hook->data, address);
	}
}

//MARK: - LCD Hooks

hook_t hook_lcd(hooks_lcd_t hooks, hook_lcd_t const hook) {
	return hooks_insert((hook_list_t)hooks, (struct hook_callback *)hook);
}

hook_t hook_lcd_emplace(hooks_lcd_t hooks, void *data, hook_lcd_callback_t callback) {
	struct hook_lcd hook = (struct hook_lcd){
		.data = data,
		.callback = callback,
	};
	return hook_lcd(hooks, &hook);
}

void hook_lcd_trigger(hooks_lcd_t hooks, ti_bw_lcd_t *lcd) {
	for (int i = 0; i < hooks->count; i++) {
		hook_lcd_t hook = &hooks->storage[i];
		hook->callback(hook->data, lcd);
	}
}
