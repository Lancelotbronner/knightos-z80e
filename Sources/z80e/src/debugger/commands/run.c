#include <string.h>

#include <z80e/ti/asic.h>
#include <z80e/ti/runloop.h>
#include <z80e/debugging/commands.h>
#include <z80e/debugging/debugger.h>
#include <z80e/disassembler/disassemble.h>
#include <z80e/hardware/t6a04.h>

struct run_disassemble_state {
	struct disassemble_memory memory;
	debugger_state_t *state;
};

uint8_t run_command_read_byte(struct disassemble_memory *state, uint16_t pointer) {
	struct run_disassemble_state *dstate = (struct run_disassemble_state *)state;

	return dstate->state->asic->cpu.read_byte(dstate->state->asic->cpu.memory, pointer);
}

int run_command_write(struct disassemble_memory *state, const char *format, ...) {
	struct run_disassemble_state *dstate = (struct run_disassemble_state *)state;

	va_list list;
	va_start(list, format);

	return dstate->state->vprint(dstate->state, format, list);
}

int command_run(debugger_state_t *state, int argc, char **argv) {
	state->asic->stopped = false;
	uint16_t instructions = -1;

	struct run_disassemble_state dstate;
	dstate.memory.read_byte = run_command_read_byte;
	dstate.memory.current = state->asic->cpu.registers.PC;
	dstate.state = state;

	int oldHalted = 0;
	int isFirstInstruction = 1;

	if ((argc == 2 && strcmp(argv[1], "--help") == 0) || argc > 2) {
		state->print(state, "run [instructions] - run a specified number of instructions\n"
				" If no number is specified, the emulator will run until interrupted (^C).\n");
		return 0;
	} else if(argc == 2) {
		instructions = parse_expression_z80e(state, argv[1]);
		state->debugger->state = DEBUGGER_LONG_OPERATION;
		for (; instructions > 0; instructions--) {
			hook_execution_trigger(&state->debugger->hook.before_execution, state->asic->cpu.registers.PC);
			if (!isFirstInstruction && state->asic->stopped) {
				state->asic->stopped = false;
				break;
			}

			if (isFirstInstruction) {
				state->asic->stopped = false;
				isFirstInstruction = 0;
			}

			if (state->debugger->flags.echo) {
				if (!state->asic->cpu.halted) {
					state->print(state, "0x%04X: ", state->asic->cpu.registers.PC);
					dstate.memory.current = state->asic->cpu.registers.PC;
					parse_instruction(&(dstate.memory), run_command_write, state->debugger->flags.knightos);
					state->print(state, "\n");
				}
			} else {
				if (state->asic->cpu.halted && !oldHalted) {
					state->print(state, "CPU is halted\n");
				}
			}

			if (state->debugger->flags.echo_reg)
				print_state(&state->asic->cpu);

			oldHalted = state->asic->cpu.halted;

			int iff1 = state->asic->cpu.IFF1;
			int iff2 = state->asic->cpu.IFF2;
			if (state->debugger->flags.nointonstep) {
				state->asic->cpu.IFF1 = 0;
				state->asic->cpu.IFF2 = 0;
			}
			asic_tick_cycles(state->asic, 1);
			if (state->debugger->flags.nointonstep) {
				state->asic->cpu.IFF1 = iff1;
				state->asic->cpu.IFF2 = iff2;
			}
			hook_execution_trigger(&state->debugger->hook.after_execution, state->asic->cpu.registers.PC);
			if (state->asic->stopped) {
				state->asic->stopped = false;
				break;
			}
		}
		state->debugger->state = DEBUGGER_ENABLED;
		return 0;
	}

	state->debugger->state = DEBUGGER_LONG_OPERATION_INTERRUPTABLE;
	while (1) {
		hook_execution_trigger(&state->debugger->hook.before_execution, state->asic->cpu.registers.PC);
		if (!isFirstInstruction && state->asic->stopped) {
			state->asic->stopped = false;
			break;
		}

		if (isFirstInstruction) {
			state->asic->stopped = false;
			isFirstInstruction = 0;
		}

		if (state->debugger->flags.echo) {
			if (!state->asic->cpu.halted) {
				state->print(state, "0x%04X: ", state->asic->cpu.registers.PC);
				dstate.memory.current = state->asic->cpu.registers.PC;
				parse_instruction(&(dstate.memory), run_command_write, state->debugger->flags.knightos);
				state->print(state, "\n");
			}
		} else {
			if (state->asic->cpu.halted && !oldHalted) {
				if (state->debugger->flags.auto_on) {
					if (!((lcd_t6a04_t )state->asic->cpu.devices[0x10].data)->display_on) {
						state->asic->cpu.halted = 0;
						state->print(state, "Turned on calculator via auto_on\n");
					}
				} else {
					state->print(state, "CPU is halted\n");
				}
			}
		}

		if (state->debugger->flags.echo_reg) {
			print_state(&state->asic->cpu);
		}

		oldHalted = state->asic->cpu.halted;

		asic_tick_cycles(state->asic, 1);

		hook_execution_trigger(&state->debugger->hook.after_execution, state->asic->cpu.registers.PC);
		if (state->asic->stopped) {
			state->debugger->state = DEBUGGER_ENABLED;
			return 0;
		}
	}
	state->debugger->state = DEBUGGER_ENABLED;
	return 0;
}

int command_step(debugger_state_t *state, int argc, char **argv) {
	char *_argv[] = { "run", "1" };
	return command_run(state, 2, _argv);
}
