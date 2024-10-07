#include <z80e/debugging/command.h>
#include <z80e/debugging/commands.h>

#include <z80e/ti/asic.h>
#include <z80e/cpu/z80.h>

//MARK: - Unhalt Command

static int command_unhalt(struct debugger_state *state, void *data, int argc, char **argv) {
	if (argc != 1) {
		state->print(state, "unhalt - Unhalts the running CPU.\n");
		return 0;
	}

	z80_cpu_t cpu = &state->asic->cpu;
	cpu->halted = 0;
	return 0;
}

const struct debugger_command UnhaltCommand = {
	.name = "unhalt",
	.summary = "Unhalts the running CPU",
	.callback = command_unhalt,
};

//MARK: - Power Command

static int __command_turn_on(debugger_state_t state, void *data, int argc, char **argv) {
	if (argc != 1) {
		state->print(state, "%s - Interrupt the CPU and raise the 'on button' interrupt\n", argv[0]);
		return 0;
	}

	asic_power_press(state->asic);
	command_execute(&RunCommand, state, 2, "run", "50000");
	asic_power_release(state->asic);
	return command_execute(&RunCommand, state, 1, "run");
}

const struct debugger_command PowerCommand = {
	.name = "turn_on",
	.summary = "Raise the 'On' interrupt",
	.description = "Interrupts the CPU and raises the 'on button' interrupt.",
	.callback = __command_turn_on,
};

//MARK: - Tick Command

static int __command_tick(struct debugger_state *state, void *data, int argc, char **argv) {
	state->asic->interrupts.interrupted.first_crystal = true;
	state->asic->cpu.interrupt = !state->asic->cpu.interrupt;
	return 0;
}

const struct debugger_command TickCommand = {
	.name = "tick",
	.summary = "Simulates a crystal tick",
	.callback = __command_tick,
};
