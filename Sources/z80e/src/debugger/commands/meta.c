//
//  commands/meta.c
//  z80e
//
//  Created by Christophe Bronner on 2024-09-20.
//

#include <z80e/debugging/commands.h>
#include <z80e/debugging/debugger.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//MARK: - List Command

static int command_list(debugger_state_t state, void *data, int argc, char **argv) {
	if (argc != 1) {
		state->print(state,
			"list_commands - List all registered commands\nThis command takes no arguments.\n");
		return 0;
	}

	for (int i = 0; i < state->debugger->commands.count; i++) {
		state->print(state, "%d. %s\n", i, state->debugger->commands.storage[i]->name);
	}
	return 0;
}

const struct debugger_command ListCommand = {
	.name = "list",
	.summary = "List all registered commands",
	.callback = command_list,
};

//MARK: - Help Command

const struct debugger_command HelpCommand = {
	.name = "help",
	.usage = "[command]",
	.summary = "Provides help with commands.",
	.callback = command_list, //TODO: command_help
};

const struct debugger_command HelpCommand1 = {
	.name = "?",
	.callback = HelpCommand.callback,
	.hidden = true,
};

//MARK: - Source Command

static int command_source(debugger_state_t state, void *data, int argc, char **argv) {
	if (argc != 2) {
		state->print(state, "%s `file` - read a file and run its commands\n", argv[0]);
		return 0;
	}

	FILE *rc = fopen(argv[1], "r");
	if (rc == 0) {
		state->print(state, "File couldn't be read: '%s'\n", strerror(errno));
		return 1;
	}
	char filebuffer[256];
	while(fgets(filebuffer, 256, rc)) {
		filebuffer[strlen(filebuffer) - 1] = 0; // drop the \n at the end
		if (filebuffer[0] == '#' || filebuffer[0] == 0) {
			continue;
		}

		if (debugger_exec(state, filebuffer) < 0) {
			return 1;
		}
	}
	return 0;
}

const struct debugger_command SourceCommand = {
	.name = "source",
	.usage = "<file>",
	.summary = "Read a file and run its commands.",
	.callback = command_source,
};

//MARK: - Set Command

static int command_set(debugger_state_t state, void *data, int argc, char **argv) {
	if (argc != 2) {
		state->print(state, "%s Set a setting. Available settings: \n 1. echo \n 2. echo_reg \n 3. auto_on \n 4. knightos \n 5. nointonstep \n", argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "echo") == 0) {
		state->debugger->flags.echo = !state->debugger->flags.echo;
	} else if (strcmp(argv[1], "echo_reg") == 0) {
		state->debugger->flags.echo_reg = !state->debugger->flags.echo_reg;
	} else if (strcmp(argv[1], "auto_on") == 0) {
		state->debugger->flags.auto_on = !state->debugger->flags.auto_on;
	} else if (strcmp(argv[1], "knightos") == 0) {
		state->debugger->flags.knightos = !state->debugger->flags.knightos;
	} else if (strcmp(argv[1], "nointonstep") == 0) {
		state->debugger->flags.nointonstep = !state->debugger->flags.nointonstep;
	} else {
		state->print(state, "Unknown variable '%s'!\n", argv[1]);
		return 1;
	}

	return 0;
}

const struct debugger_command SetCommand = {
	.name = "set",
	.usage = "(echo|echo_reg|auto_on|knightos|nointonstep)",
	.summary = "Set a setting.",
	.callback = command_set,
};

//MARK: - Unset Command

static int command_unset(debugger_state_t state, void *data, int argc, char **argv) {
	if (argc != 2) {
		state->print(state, "%s `val` - unset a setting\n", argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "echo") == 0) {
		state->debugger->flags.echo = 0;
	} else if (strcmp(argv[1], "echo_reg") == 0) {
		state->debugger->flags.echo_reg = 0;
	} else if (strcmp(argv[1], "auto_on") == 0) {
		state->debugger->flags.auto_on = 0;
	} else if (strcmp(argv[1], "knightos") == 0) {
		state->debugger->flags.knightos = 0;
	} else if (strcmp(argv[1], "nointonstep") == 0) {
		state->debugger->flags.nointonstep = 0;
	} else {
		state->print(state, "Unknown variable '%s'!\n", argv[1]);
		return 1;
	}

	return 0;
}

const struct debugger_command UnsetCommand = {
	.name = "unset",
	.usage = "<echo|echo_reg|auto_on|knightos|nointonstep>",
	.summary = "Unset a setting.",
	.callback = command_unset,
};
