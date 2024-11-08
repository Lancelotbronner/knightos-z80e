#include <z80e/debugging/command.h>
#include <z80e/debugging/commands.h>
#include <z80e/debugging/debugger.h>

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//MARK: - Command Management

//TODO: Be able to group commands?
//TODO: Split the concepts of commands, names (aliases) and usages?

bool debugger_register(debugger_t debugger, const debugger_command_t command, void *data) {
	if (!command || !command->callback)
		return;

	if (debugger->commands.count >= debugger->commands.capacity) {
		debugger->commands.capacity += 8;
		void *storage = malloc(debugger->commands.capacity * sizeof(debugger_command_t));
		memcpy(storage, debugger->commands.storage, debugger->commands.count * sizeof(debugger_command_t));
		debugger->commands.storage = storage;
	}

	debugger->commands.storage[debugger->commands.count++] = command;
}

//MARK: - Debugger Initialization

static const debugger_command_t DebuggerCommands[] = {
	// CLI Commands
	&ListCommand,
	&HelpCommand, &HelpCommand1,

	// Source Commands
	&SourceCommand,

	// Port Commands
	&InCommand,
	&OutCommand,

	// Debugging Commands
	&BreakCommand,
	&RunCommand,
	&StepCommand,
	&StepOverCommand,
	//TODO: Step over has alternate `so` alias.
	&StopCommand,

	// Print Commands
	&PrintCommand,

	// Screen Commands
	&DumpScreenCommand,

	// Memory Commands
	&DumpMappingsCommand,
	&HexdumpCommand,
	&HexdumpBackwardsCommand,

	// Disassembly Commands
	&DisassembleCommand,

	// Debugger Commands
	&SetCommand,
	&UnsetCommand,
	&OnCommand,

	// Interrupt Commands
	&UnhaltCommand,
	&PowerCommand,
	&TickCommand,

	// Link Commands
	&SendCommand,
	&ReceiveCommand,
	&ConnectCommand,
	&DumpLinkCommand,

	// Keyboard Commands
	&PressCommand,
	&ReleaseCommand,
	&TapCommand,

	// z80 Commands
	&LoadCommand,
	&DumpRegisterCommand,
	//TODO: print register has registers and regs as aliases
	&DumpStackCommand,

	// Timer Commands
	&TimerCommand,
};

void debugger_init(debugger_t debugger, asic_t asic) {
	int length = sizeof DebuggerCommands / sizeof *DebuggerCommands;
	*debugger = (struct debugger) {
		.asic = asic,
		.commands.count = length,
		.commands.capacity = length,
		.state = DEBUGGER_DISABLED,
	};

	debugger->commands.storage = malloc(length * sizeof(debugger_command_t));
	for (int i = 0; i < length; i++)
		debugger->commands.storage[i] = DebuggerCommands[i];
}

void debugger_deinit(debugger_t debugger) {
	free(debugger->commands.storage);
}

int debugger_source(debugger_t debugger, const char *rc_name) {
	char *env = getenv("XDG_CONFIG_HOME");
	char *realloced;
	size_t strsize = 0;
	if (!env) {
		env = getenv("HOME");
		size_t hsize = strlen(env);
		realloced = malloc(hsize + 9);
		strcpy(realloced, env);
		strcpy(realloced + hsize, "/.config");
		strsize = hsize + 8;
	} else {
		strsize = strlen(env);
		realloced = malloc(strsize + 1);
		strcpy(realloced, env);
	}

	realloced = realloc(realloced, strsize + strlen(rc_name) + 2);
	strcpy(realloced + strsize, "/");
	strcpy(realloced + strsize + 1, rc_name);

	struct stat stat_buf;
	if (stat(realloced, &stat_buf) == -1) {
		if (errno != ENOENT)
			debugger_print(debugger, "Couldn't read %s: '%s'\n", rc_name, strerror(errno));
		free(realloced);
		return 0;
	}

	int ret = command_execute(&SourceCommand, debugger, 2, "source", realloced);

	//TODO: Avoid this temporary allocation.
	free(realloced);
	return ret;
}

static int compare_strings(const char *a, const char *b) {
	int i = 0;
	while (*a != 0 && *b != 0 && *(a++) == *(b++))
		i++;
	return i;
}

//MARK: - Debugger Invocation

int debugger_find(debugger_t debugger, const char *f_command, debugger_command_t * pointer) {
	int max_match = 0;
	size_t command_length = strlen(f_command);
	int highest_priority = INT_MIN;
	int highest_priority_max = 0;

	debugger_command_t best_command = 0;

	for (int i = 0; i < debugger->commands.count; i++) {
		debugger_command_t cmd = debugger->commands.storage[i];
		int match = compare_strings(f_command, cmd->name);

		if (command_length > strlen(cmd->name))
			continue;

		if (strlen(f_command) != match && match < command_length)
			continue;

		if (match < max_match)
			continue;

		if (match > max_match) {
			max_match = match;
			best_command = cmd;
		}

		// we found a duplicate
		return -1;
	}

	*pointer = best_command;
	if (max_match || (max_match && highest_priority_max < 1))
		return 1;

	if (max_match == 0)
		return 0;

	return 0;
}

static char **debugger_parse(const char *cmdline, int *argc) {
	//TODO: Simplify and optimize debugger_parse
	// Note: Write n values to provided array so I can reuse it
	char *buffer[10];
	int buffer_pos = 0;
	while (*cmdline != 0 && *cmdline != '\n') {
		int length = 0;
		int is_string = 0;
		const char *tmp = cmdline;
		while ((is_string || *tmp != ' ') && *tmp != '\n' && *tmp) {
			length++;
			if (*tmp == '\\' && is_string) {
				tmp++;
			} else if(*tmp == '"') {
				is_string = !is_string;
				tmp++;
			}

			tmp++;
		}
		is_string = 0;

		buffer[buffer_pos] = malloc(length + 1);

		memset(buffer[buffer_pos], 0, length + 1);

		tmp = cmdline;
		char *tmp2 = buffer[buffer_pos];
		while ((is_string || *tmp != ' ') && *tmp != '\n' && *tmp) {
			if (*tmp == '\\' && is_string) {
				tmp++;
				switch (*tmp) {
				case 't':
					*(tmp2++) = '\t';
					break;
				case 'n':
					*(tmp2++) = '\n';
					break;
				case 'r':
					*(tmp2++) = '\r';
					break;
				default:
					*(tmp2++) = *tmp;
				}
			} else if (*tmp == '"') {
				is_string = !is_string;
			} else {
				*(tmp2++) = *tmp;
			}
			tmp++;
		}

		cmdline = tmp;
		while (*cmdline == ' ') {
			cmdline++;
		}

		buffer_pos++;
	}
	char **result = malloc((buffer_pos + 1) * sizeof(char *));

	int i;
	for (i = 0; i < buffer_pos; i++) {
		result[i] = buffer[i];
	}
	result[buffer_pos] = 0;

	*argc = buffer_pos;
	return result;
}

int debugger_execute(debugger_t debugger, const char *command_str) {
	debugger_command_t command;
	int argc;
	char **argv = debugger_parse(command_str, &argc);
	int return_value = 0;

	int status = debugger_find(debugger, argv[0], &command);
	if (status == -1) {
		debugger_print(debugger, "Error: Ambiguous command %s\n", argv[0]);
		return_value = -1;
	} else if (status == 0) {
		debugger_print(debugger, "Error: Unknown command %s\n", argv[0]);
		return_value = -2;
	} else {
		return_value = command_executev(command, debugger, argc, argv);
	}

	char **tmp = argv;
	while (*tmp)
		free(*(tmp++));
	free(argv);

	return return_value;
}

//MARK: - Debugger Utilities

int debugger_print(debugger_t debugger, const char *format, ...) {
	va_list args;
	va_start(args, format);
	debugger->vprint(debugger, format, args);
	va_end(args);
}

int debugger_vprint(debugger_t debugger, const char *format, va_list args) {
	debugger->vprint(debugger, format, args);
}
