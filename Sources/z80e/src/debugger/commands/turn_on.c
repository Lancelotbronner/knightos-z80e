#include <z80e/debugging/debugger.h>
#include <z80e/debugging/commands.h>

#include <z80e/ti/asic.h>

int command_turn_on(debugger_state_t *state, int argc, char **argv) {
	if (argc != 1) {
		state->print(state, "%s - Interrupt the CPU and raise the 'on button' interrupt\n", argv[0]);
		return 0;
	}

	asic_power_press(state->asic);
	char *_argv[] = { "run", "50000" };
	command_run(state, 2, _argv);
	asic_power_release(state->asic);
	char *__argv[] = { "run" };
	return command_run(state, 1, __argv);
}
