#include <z80e/debugging/command.h>
#include <z80e/debugging/commands.h>

#include <z80e/ti/asic.h>
#include <z80e/cpu/z80.h>

//MARK: - Unhalt Command

static int command_unhalt(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc != 1) {
		debugger_print(debugger, "unhalt - Unhalts the running CPU.\n");
		return 0;
	}

	z80_cpu_t cpu = &debugger->asic->cpu;
	cpu->halted = 0;
	return 0;
}

const struct debugger_command UnhaltCommand = {
	.name = "unhalt",
	.summary = "Unhalts the running CPU",
	.callback = command_unhalt,
};

//MARK: - Power Command

static int __command_turn_on(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc != 1) {
		debugger_print(debugger, "%s - Interrupt the CPU and raise the 'on button' interrupt\n", argv[0]);
		return 0;
	}

	asic_power_press(debugger->asic);
	command_execute(&RunCommand, debugger, 2, "run", "50000");
	asic_power_release(debugger->asic);
	return command_execute(&RunCommand, debugger, 1, "run");
}

const struct debugger_command PowerCommand = {
	.name = "turn_on",
	.summary = "Raise the 'On' interrupt",
	.description = "Interrupts the CPU and raises the 'on button' interrupt.",
	.callback = __command_turn_on,
};

//MARK: - Tick Command

static int __command_tick(debugger_t debugger, void *data, int argc, char **argv) {
	debugger->asic->interrupts.interrupted.first_crystal = true;
	debugger->asic->cpu.interrupt = !debugger->asic->cpu.interrupt;
	return 0;
}

const struct debugger_command TickCommand = {
	.name = "tick",
	.summary = "Simulates a crystal tick",
	.callback = __command_tick,
};
