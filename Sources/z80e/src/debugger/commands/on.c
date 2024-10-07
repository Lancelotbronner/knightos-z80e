#include <z80e/debugging/commands.h>
#include <z80e/debugging/debugger.h>
#include <z80e/disassembler/disassemble.h>
#include <z80e/log.h>
#include <z80e/ti/asic.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//MARK: - On Command

//TODO: Formal breakpoints
// Formalize breakpoints in the debugger and allow the following:
// - Have the concept of an current breakpoint
// - Create breakpoint with optional label (becomes current)
// - List breakpoints (note current one)
// - (stretch) Add conditions to current breakpoint?
// - Enable/Disable current breakpoint
// - Set number of times to ignore current breakpoint before triggering
// - Set number of times to break on current breakpoint before being automatically deactivated
// - Add actions to current breakpoint
// - Toggle whether or not the current breakpoint should continue after evaluating actions

enum {
	REGISTER,
	MEMORY,
	EXECUTION,
	PORT
};

enum {
	READ = (1 << 0),
	WRITE = (1 << 1)
};

static int register_from_string(char *string) {
	#define REGISTER(num, len, print) if (strncasecmp(string, print, len) == 0) return num;
	REGISTER(A, 1, "A");
	REGISTER(B, 1, "B");
	REGISTER(C, 1, "C");
	REGISTER(D, 1, "D");
	REGISTER(E, 1, "E");
	REGISTER(F, 1, "F");
	REGISTER(H, 1, "H");
	REGISTER(L, 1, "L");
	REGISTER(AF, 2, "AF");
	REGISTER(_AF, 3, "AF'");
	REGISTER(BC, 2, "BC");
	REGISTER(_BC, 3, "BC'");
	REGISTER(DE, 2, "DE");
	REGISTER(_DE, 3, "DE'");
	REGISTER(HL, 2, "HL");
	REGISTER(_HL, 3, "HL'");
	REGISTER(PC, 2, "PC");
	REGISTER(SP, 2, "SP");
	REGISTER(I, 1, "I");
	REGISTER(R, 1, "R");
	REGISTER(IXH, 3, "IXH");
	REGISTER(IXL, 3, "IXL");
	REGISTER(IX, 2, "IX");
	REGISTER(IYH, 3, "IYH");
	REGISTER(IYL, 3, "IYL");
	REGISTER(IY, 2, "IY");

	return -1;
}

typedef struct {
	int look_for;
	debugger_state_t deb_sta;
	char *exec_string;
} on_state_t;

static uint16_t command_on_register_hook(void *state, enum z80_registers reg, uint16_t value) {
	on_state_t *data = state;
	debugger_exec(data->deb_sta, data->exec_string);
	return value;
}

static uint8_t command_on_memory_hook(void *state, uint16_t location, uint8_t value) {
	on_state_t *data = state;
	debugger_exec(data->deb_sta, data->exec_string);
	return value;
}

static uint8_t command_on_port_hook(void *state, uint8_t port, uint8_t value) {
	on_state_t *data = state;
	debugger_exec(data->deb_sta, data->exec_string);
	return value;
}

static int command_on(struct debugger_state *state, void *data, int argc, char **argv) {
	if (argc < 5) {
		printf("%s `type` `read|write` `value` `command`\n"
			" Run a command on a specific case\n"
			" The type can be memory / register\n"
			" The value can be a register, or an expression\n",
			argv[0]);
		return 0;
	}

	int thing = 0;
	if (*argv[2] == 'r')
		thing = READ;

	if (*argv[2] == 'w')
		thing = WRITE;

	if (strncasecmp(argv[2], "rw", 2) == 0)
		thing = READ | WRITE;

	if (thing == 0) {
		state->print(state, "ERROR: First argument must be read, write, or rw\n");
		return 1;
	}

	on_state_t *sta = malloc(sizeof(on_state_t));
	sta->deb_sta = state->create_new_state(state, argv[4]);
	sta->exec_string = malloc(strlen(argv[4]) + 1);
	strcpy(sta->exec_string, argv[4]);

	if (strncasecmp(argv[1], "register", 8) == 0) {
		sta->look_for = register_from_string(argv[3]);
		if (sta->look_for == -1) {
			state->print(state, "ERROR: Invalid register!\n");
			free(sta);
			return 1;
		}
		if (thing & READ)
			hook_register_emplace(&state->asic->cpu.hook.register_read, sta->look_for, sta, command_on_register_hook);
		if (thing & WRITE)
			hook_register_emplace(&state->asic->cpu.hook.register_write, sta->look_for, sta, command_on_register_hook);
	} else if (strncasecmp(argv[1], "memory", 6) == 0) {
		sta->look_for = debugger_evaluate(state, argv[3]);
		if (thing & READ)
			hook_memory_emplace(&state->asic->mmu.hook.memory_read, sta->look_for, sta->look_for, sta, command_on_memory_hook);
		if (thing & WRITE)
			hook_memory_emplace(&state->asic->mmu.hook.memory_write, sta->look_for, sta->look_for, sta, command_on_memory_hook);
	} else if (strncasecmp(argv[1], "port", 4) == 0) {
		sta->look_for = debugger_evaluate(state, argv[3]);
		if (thing & READ)
			hook_port_emplace(&state->asic->cpu.hook.port_in, sta->look_for, sta->look_for, sta, command_on_port_hook);
		if (thing & WRITE)
			hook_port_emplace(&state->asic->cpu.hook.port_out, sta->look_for, sta->look_for, sta, command_on_port_hook);
	} else {
		free(sta);
		state->print(state, "ERROR: Second argument must be memory, register or port!\n");
		return 1;
	}

	return 0;
}

const struct debugger_command OnCommand = {
	.name = "on",
	.usage = "on (register|memory|port) (r|w|rw) <value|expression> <command...>",
	.summary = "Hooks a command to a breakpoint.",
	.callback = command_on,
};
