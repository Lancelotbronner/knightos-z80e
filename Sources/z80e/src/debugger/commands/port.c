#include <z80e/debugging/debugger.h>
#include <z80e/debugging/commands.h>

#include <z80e/ti/asic.h>

//MARK: - In Command

static int command_in(debugger_t state, void *data, int argc, char **argv) {
	if(argc != 2) {
		debugger_print(state, "%s `port` - read from port `port`.\n", argv[0]);
		return 0;
	}

	uint8_t port = debugger_evaluate(state, argv[1]) & 0xFF;
	uint8_t val = device_read(&state->asic->cpu.devices[port]);
	debugger_print(state, "port 0x%02X -> 0x%02X\n", port, val);
	return 0;
}

const struct debugger_command InCommand = {
	.name = "in",
	.usage = "<port>",
	.summary = "Reads from port",
	.callback = command_in,
};

//MARK: - Out Command

int command_out(debugger_t state, void *data, int argc, char **argv) {
	if(argc != 3) {
		debugger_print(state, "%s `port` `value` - write `value` into port `port`.\n", argv[0]);
		return 0;
	}

	uint8_t port = debugger_evaluate(state, argv[1]) & 0xFF;
	uint8_t val = debugger_evaluate(state, argv[2]) & 0xFF;
	device_write(&state->asic->cpu.devices[port], val);
	debugger_print(state, "port 0x%02X <- 0x%02X\n", port, val);
	return 0;
}

const struct debugger_command OutCommand = {
	.name = "out",
	.usage = "<port>",
	.summary = "Writes to port",
	.callback = command_out,
};
