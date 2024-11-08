#include <z80e/debugging/debugger.h>
#include <z80e/debugging/commands.h>

#include <z80e/ti/asic.h>

//MARK: - In Command

static int command_in(debugger_t debugger, void *data, int argc, char **argv) {
	if(argc != 2) {
		debugger_print(debugger, "%s `port` - read from port `port`.\n", argv[0]);
		return 0;
	}

	uint8_t port = debugger_evaluate(debugger, argv[1]) & 0xFF;
	uint8_t val = device_read(&debugger->asic->cpu.devices[port]);
	debugger_print(debugger, "port 0x%02X -> 0x%02X\n", port, val);
	return 0;
}

const struct debugger_command InCommand = {
	.name = "in",
	.usage = "<port>",
	.summary = "Reads from port",
	.callback = command_in,
};

//MARK: - Out Command

int command_out(debugger_t debugger, void *data, int argc, char **argv) {
	if(argc != 3) {
		debugger_print(debugger, "%s `port` `value` - write `value` into port `port`.\n", argv[0]);
		return 0;
	}

	uint8_t port = debugger_evaluate(debugger, argv[1]) & 0xFF;
	uint8_t val = debugger_evaluate(debugger, argv[2]) & 0xFF;
	device_write(&debugger->asic->cpu.devices[port], val);
	debugger_print(debugger, "port 0x%02X <- 0x%02X\n", port, val);
	return 0;
}

const struct debugger_command OutCommand = {
	.name = "out",
	.usage = "<port>",
	.summary = "Writes to port",
	.callback = command_out,
};
