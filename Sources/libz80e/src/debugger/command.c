//
//  command.c
//  z80e
//
//  Created by Christophe Bronner on 2024-10-07.
//

#include <z80e/debugging/command.h>

#include <stdarg.h>

//MARK: - Command Execution

int command_executev(debugger_command_t command, debugger_t debugger, int argc, char **argv) {
	return command->callback(debugger, command->data, argc, argv);
}

int command_execute(debugger_command_t command, debugger_t debugger, int argc, ...) {
	va_list argp;
	va_start(argp, format);
	char *argv[argc];
	for (int i = 0; i < argc; i++)
		argv[i] = va_arg(argp, char*);
	va_end(argp);

	return command->callback(debugger, command->data, argc, argv);
}
