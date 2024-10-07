#pragma once

#include <z80e/types.h>
#include <z80e/debugging/hooks.h>

#include <stdarg.h>

typedef int (*debugger_vprint_t)(debugger_t debugger, const char *format, va_list args);
typedef void (*debugger_open_t)(debugger_t debugger, const char *command);
typedef void (*debugger_callback_t)(debugger_t debugger);

struct debugger_commands {
	int count;
	int capacity;
	debugger_command_t *storage;
};

enum debugger_op_state {
	DEBUGGER_DISABLED,
	DEBUGGER_ENABLED,
	DEBUGGER_LONG_OPERATION,
	DEBUGGER_LONG_OPERATION_INTERRUPTABLE
};

struct debugger {
	debugger_callback_t deinit;
	debugger_vprint_t vprint;
	debugger_open_t open;
	debugger_callback_t close;
	//TODO: is open/close even needed?

	asic_t asic;
	void *data;

	struct debugger_commands commands;

	struct {
		struct hooks_execution before_execution;
		struct hooks_execution after_execution;
	} hook;

	struct {
		bool echo : 1;
		bool echo_reg : 1;
		bool auto_on : 1;
		bool knightos : 1;
		bool nointonstep : 1;
	} flags;

	enum debugger_op_state state : 2;
};

int debugger_source(debugger_t , const char *rc_name);

void debugger_init(debugger_t debugger, asic_t asic);
void debugger_deinit(debugger_t debugger);

int debugger_find(debugger_t debugger, const char *name, debugger_command_t *result);
bool debugger_register(debugger_t debugger, const debugger_command_t command, void *data);

int debugger_print(debugger_t debugger, const char *format, ...);
int debugger_vprint(debugger_t debugger, const char *format, va_list args);
int debugger_execute(debugger_t debugger, const char *command);
uint16_t debugger_evaluate(debugger_t debugger, const char *expression);
