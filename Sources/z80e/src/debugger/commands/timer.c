#include <z80e/debugging/commands.h>
#include <z80e/debugging/debugger.h>
#include <z80e/ti/asic.h>

#include <string.h>

//MARK: - Timer Command

//TODO: Understand and document the timer command

static int command_timer(debugger_t state, void *data, int argc, char **argv) {
	if (argc < 2) {
		debugger_print(state, "%s [port] [value]\n", argv[0]);
		return 0;
	}

	uint8_t port = debugger_evaluate(state, argv[2]);
	uint8_t val = debugger_evaluate(state, argv[3]);

	uint8_t *timer = asic_device(state->asic, port)->data;
	timer[(port - 0x30) % 3] = val;

	return 0;
}

const struct debugger_command TimerCommand = {
	.name = "timer",
	.usage = "<port> <value>",
	.summary = "Loads a value into a timer",
	.callback = command_timer,
};
