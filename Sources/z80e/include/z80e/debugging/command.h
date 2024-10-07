//
//  command.h
//  debugging/z80e
//
//  Created by Christophe Bronner on 2024-09-20.
//

#pragma once

#include <z80e/types.h>

typedef int (*command_callback_t)(debugger_t state, void *data, int argc, char **argv);

struct debugger_command {
	const char *name;
	const char *usage;
	const char *summary;
	const char *description;

	const command_callback_t callback;
	void *data;

	bool hidden : 1;
};

int command_executev(debugger_command_t command, debugger_t debugger, int argc, char **argv);
int command_execute(debugger_command_t command, debugger_t debugger, int argc, ...);
