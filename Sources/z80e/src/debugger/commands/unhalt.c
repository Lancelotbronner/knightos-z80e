#include <z80e/debugger/debugger.h>
#include <z80e/cpu/z80.h>

int command_unhalt(struct debugger_state *state, int argc, char **argv) {
	if (argc != 1) {
		state->print(state, "unhalt - Unhalts the running CPU.\n");
		return 0;
	}

	z80_cpu_t cpu = &state->asic->cpu;
	cpu->halted = 0;
	return 0;
}

