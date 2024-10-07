#include <z80e/ti/asic.h>
#include <z80e/ti/runloop.h>
#include <z80e/debugging/commands.h>
#include <z80e/debugging/debugger.h>
#include <z80e/disassembler/disassemble.h>
#include <z80e/hardware/t6a04.h>
#include <z80e/log.h>

#include <stdlib.h>
#include <string.h>

//MARK: - Run Command

struct run_disassemble_state {
	struct disassemble_memory memory;
	debugger_t state;
};

static uint8_t __run_read(struct disassemble_memory *state, uint16_t pointer) {
	struct run_disassemble_state *dstate = (struct run_disassemble_state *)state;

	return dstate->state->asic->cpu.read_byte(dstate->state->asic->cpu.memory, pointer);
}

static int __run_write(struct disassemble_memory *state, const char *format, ...) {
	struct run_disassemble_state *dstate = (struct run_disassemble_state *)state;

	va_list list;
	va_start(list, format);

	return debugger_vprint(dstate->state, format, list);
}

static int __command_run(debugger_t state, void *data, int argc, char **argv) {
	state->asic->stopped = false;
	uint16_t instructions = -1;

	struct run_disassemble_state dstate;
	dstate.memory.read_byte = __run_read;
	dstate.memory.current = state->asic->cpu.registers.PC;
	dstate.state = state;

	int oldHalted = 0;
	int isFirstInstruction = 1;

	if ((argc == 2 && strcmp(argv[1], "--help") == 0) || argc > 2) {
		debugger_print(state, "run [instructions] - run a specified number of instructions\n"
				" If no number is specified, the emulator will run until interrupted (^C).\n");
		return 0;
	}

	if(argc == 2) {
		instructions = debugger_evaluate(state, argv[1]);
		state->state = DEBUGGER_LONG_OPERATION;

		for (; instructions > 0; instructions--) {
			hook_execution_trigger(&state->hook.before_execution, state->asic->cpu.registers.PC);
			if (!isFirstInstruction && state->asic->stopped) {
				state->asic->stopped = false;
				break;
			}

			if (isFirstInstruction) {
				state->asic->stopped = false;
				isFirstInstruction = 0;
			}

			if (state->flags.echo) {
				if (!state->asic->cpu.halted) {
					debugger_print(state, "0x%04X: ", state->asic->cpu.registers.PC);
					dstate.memory.current = state->asic->cpu.registers.PC;
					parse_instruction(&(dstate.memory), __run_write, state->flags.knightos);
					debugger_print(state, "\n");
				}
			} else if (state->asic->cpu.halted && !oldHalted) {
				debugger_print(state, "CPU is halted\n");
			}

			if (state->flags.echo_reg)
				print_state(&state->asic->cpu);

			oldHalted = state->asic->cpu.halted;

			int iff1 = state->asic->cpu.IFF1;
			int iff2 = state->asic->cpu.IFF2;
			if (state->flags.nointonstep) {
				state->asic->cpu.IFF1 = 0;
				state->asic->cpu.IFF2 = 0;
			}
			asic_tick_cycles(state->asic, 1);
			if (state->flags.nointonstep) {
				state->asic->cpu.IFF1 = iff1;
				state->asic->cpu.IFF2 = iff2;
			}
			hook_execution_trigger(&state->hook.after_execution, state->asic->cpu.registers.PC);
			if (state->asic->stopped) {
				state->asic->stopped = false;
				break;
			}
		}
		state->state = DEBUGGER_ENABLED;
		return 0;
	}

	state->state = DEBUGGER_LONG_OPERATION_INTERRUPTABLE;
	while (1) {
		hook_execution_trigger(&state->hook.before_execution, state->asic->cpu.registers.PC);

		if (!isFirstInstruction && state->asic->stopped) {
			state->asic->stopped = false;
			break;
		}

		if (isFirstInstruction) {
			state->asic->stopped = false;
			isFirstInstruction = 0;
		}

		if (state->flags.echo) {
			if (!state->asic->cpu.halted) {
				debugger_print(state, "0x%04X: ", state->asic->cpu.registers.PC);
				dstate.memory.current = state->asic->cpu.registers.PC;
				parse_instruction(&(dstate.memory), __run_write, state->flags.knightos);
				debugger_print(state, "\n");
			}
		} else if (state->asic->cpu.halted && !oldHalted) {
			if (state->flags.auto_on) {
				if (!state->asic->lcd.display_on) {
					state->asic->cpu.halted = 0;
					debugger_print(state, "Turned on calculator via auto_on\n");
				}
			} else {
				debugger_print(state, "CPU is halted\n");
			}
		}

		if (state->flags.echo_reg)
			print_state(&state->asic->cpu);

		oldHalted = state->asic->cpu.halted;

		asic_tick_cycles(state->asic, 1);

		hook_execution_trigger(&state->hook.after_execution, state->asic->cpu.registers.PC);
		if (state->asic->stopped) {
			state->state = DEBUGGER_ENABLED;
			return 0;
		}
	}
	state->state = DEBUGGER_ENABLED;
	return 0;
}

const struct debugger_command RunCommand = {
	.name = "run",
	.usage = "[count]",
	.summary = "Executes instructions",
	.description =
	"Runs a specified number of instructions." "\n"
	"If no `count` is specified, the emulator will run until interrupted (^C).",
	.callback = __command_run,
};

//MARK: - Break Command

struct break_data {
	asic_t asic;
	hook_t hook;
	uint16_t address;
	int16_t count;
};

static void break_callback(void *data, uint16_t address) {
	struct break_data *breakpoint;

	if(breakpoint->address != address)
		return;

	z80e_debug("break", "Breakpoint hit at 0x%04X", address);
	breakpoint->asic->stopped = true;

	if (breakpoint->count != -1 && !(--breakpoint->count))
		hook_cancel(breakpoint->hook);
}

static int __command_break(debugger_t state, void *data, int argc, char **argv) {
	if (argc != 2 && argc != 3) {
		debugger_print(state, "%s `address` [count] - break at address\n", argv[0]);
		return 0;
	}

	uint16_t address = debugger_evaluate(state, argv[1]);

	int count = -1;
	if (argc == 3)
		count = debugger_evaluate(state, argv[2]);

	struct break_data *breakpoint = malloc(sizeof(struct break_data));
	breakpoint->address = address;
	breakpoint->asic = state->asic;
	breakpoint->count = count;
	breakpoint->hook = hook_execution_emplace(&state->hook.before_execution, data, break_callback);
	return 0;
}

const struct debugger_command BreakCommand = {
	.name = "break",
	.usage = "<address> [count]",
	.summary = "Breaks at address",
	.callback = __command_break,
};

//MARK: - Step Command

static int __command_step(debugger_t state, void *data, int argc, char **argv) {
	return command_execute(&RunCommand, state, "run", "1");
}

const struct debugger_command StepCommand = {
	.name = "step",
	.summary = "Executes a single instruction",
	.callback = __command_step,
};

//MARK: - Step Over Command

typedef struct {
	ti_mmu_t mmu;
	debugger_t state;
} command_step_over_dism_extra_t;

static uint8_t __stepover_read(struct disassemble_memory *dmem, uint16_t mem) {
	command_step_over_dism_extra_t *extra = dmem->extra_data;
	return ti_read_byte(extra->mmu, mem);
}

static int __stepover_write(struct disassemble_memory *mem, const char *thing, ...) {
	command_step_over_dism_extra_t *extra = mem->extra_data;
	if (extra->state->flags.echo) {
		va_list list;
		va_start(list, thing);
		return debugger_vprint(extra->state, thing, list);
	}
	return 0;
}

static void __stepover_callback(void *data, uint16_t address) {
	struct break_data *breakpoint;

	if(breakpoint->address != address)
		return;

	breakpoint->asic->stopped = true;
	hook_cancel(breakpoint->hook);
}

static int __command_stepover(debugger_t state, void *data, int argc, char **argv) {
	if (argc != 1) {
		debugger_print(state, "%s - set a breakpoint for the instruction after the current one\n", argv[0]);
		return 0;
	}
	command_step_over_dism_extra_t extra = { &state->asic->mmu, state };
	struct disassemble_memory mem = { __stepover_read, state->asic->cpu.registers.PC, &extra };

	if (state->flags.echo)
		debugger_print(state, "0x%04X: ", state->asic->cpu.registers.PC);

	uint16_t size = parse_instruction(&mem, __stepover_write, state->flags.knightos);
	if (state->flags.echo)
		debugger_print(state, "\n");

	// Note: 0x18, 0xFE is JR $, i.e. an infinite loop, which we step over as a special case
	const uint8_t jumps[] = { 0x18, 0x28, 0x38, 0x30, 0x20 };
	int i;
	for (i = 0; i < sizeof(jumps) / sizeof(uint8_t); ++i)
		if (cpu_read_byte(&state->asic->cpu, state->asic->cpu.registers.PC) == jumps[i] &&
			cpu_read_byte(&state->asic->cpu, state->asic->cpu.registers.PC + 1) == 0xFE) {
			state->asic->cpu.registers.PC += 2;
			return 0;
		}

	if (state->flags.knightos)
		if (cpu_read_byte(&state->asic->cpu, state->asic->cpu.registers.PC) == 0xE7)
			size += 2;

	struct break_data *breakpoint = malloc(sizeof(struct break_data));
	breakpoint->address = state->asic->cpu.registers.PC + size;
	breakpoint->asic = state->asic;
	breakpoint->hook = hook_execution_emplace(&state->hook.before_execution, data, __stepover_callback);

	int orig_echo = state->flags.echo;
	state->flags.echo = false;

	int val = command_execute(&RunCommand, state, 1, "run");
	state->flags.echo = orig_echo;
	return val;
}

const struct debugger_command StepOverCommand = {
	.name = "step_over",
	.summary = "Breaks on the next instruction",
	.description = "Sets a breakpoint for the instruction after the current one",
	.callback = __command_stepover,
};

//MARK: - Stop Command

static int __command_stop(debugger_t state, void *data, int argc, char **argv) {
	state->asic->stopped = true;
	return 0;
}

const struct debugger_command StopCommand = {
	.name = "stop",
	.summary = "Stops the processor",
	.callback = __command_stop,
};
