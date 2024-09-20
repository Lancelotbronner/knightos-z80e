#pragma once

#include <z80e/types.h>
#include <z80e/debugging/hooks.h>

#include <stdarg.h>

typedef struct debugger_state debugger_state_t;
typedef int (*debugger_function_t)(debugger_state_t *, int, char **);

typedef struct {
	const char *name;
	debugger_function_t function;
	int priority;
	void *state;
} debugger_command_t;

typedef struct {
	int count;
	int capacity;
	debugger_command_t **commands;
} debugger_list_t;

struct debugger_state {
	int (*print)(debugger_state_t *, const char *, ...);
	int (*vprint)(debugger_state_t *, const char *, va_list);
	void *state;
	void *interface_state;
	asic_t asic;
	debugger_t debugger;
	debugger_state_t *(*create_new_state)(debugger_state_t *, const char *command_name);
	void (*close_window)(debugger_state_t *);
};

typedef enum {
	DEBUGGER_DISABLED,
	DEBUGGER_ENABLED,
	DEBUGGER_LONG_OPERATION,
	DEBUGGER_LONG_OPERATION_INTERRUPTABLE
} debugger_operation_state;

struct debugger {
	debugger_list_t commands;
	asic_t asic;
	debugger_operation_state state;

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
};

int debugger_source_rc(debugger_state_t *, const char *rc_name);

void debugger_init(debugger_t debugger, asic_t asic);
void debugger_deinit(debugger_t );

int debugger_find(debugger_t , const char *, debugger_command_t **);
void __debugger_register(debugger_t , const char *, debugger_function_t, void *, int);

char **debugger_parse(const char *, int *);
int debugger_exec(debugger_state_t *, const char *);
