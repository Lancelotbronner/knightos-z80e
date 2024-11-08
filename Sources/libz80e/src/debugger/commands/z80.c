#include <z80e/debugging/command.h>

#include <z80e/ti/asic.h>

#include <string.h>

//MARK: - Load Register Command

static int __command_load(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc != 3) {
		debugger_print(debugger, "%s - Load a register with a specified value. Supports hex (i.e. 0x0000) and base-10 (i.e. 12)\n", argv[0]);
		return 0;
	}

#define REGISTER(num, len, print) \
if (strncasecmp(argv[1], print, len) == 0) \
debugger->asic->cpu.registers. num = debugger_evaluate(debugger, argv[2]);

	REGISTER(IXH, 3, "IXH");
	REGISTER(IXL, 3, "IXL");
	REGISTER(IYH, 3, "IYH");
	REGISTER(IYL, 3, "IYL");
	REGISTER(_BC, 3, "BC'");
	REGISTER(_DE, 3, "DE'");
	REGISTER(_HL, 3, "HL'");
	REGISTER(_AF, 3, "AF'");
	REGISTER(IX, 2, "IX");
	REGISTER(IY, 2, "IY");
	REGISTER(AF, 2, "AF");
	REGISTER(BC, 2, "BC");
	REGISTER(DE, 2, "DE");
	REGISTER(HL, 2, "HL");
	REGISTER(PC, 2, "PC");
	REGISTER(SP, 2, "SP");
	REGISTER(A, 1, "A");
	REGISTER(B, 1, "B");
	REGISTER(C, 1, "C");
	REGISTER(D, 1, "D");
	REGISTER(E, 1, "E");
	REGISTER(F, 1, "F");
	REGISTER(H, 1, "H");
	REGISTER(L, 1, "L");
	REGISTER(I, 1, "I");
	REGISTER(R, 1, "R");

	return 0;
}

const struct debugger_command LoadCommand = {
	.name = "ld",
	.usage = "<register> <value>",
	.summary = "Loads a value into a register.",
	.callback = __command_load,
};


//MARK: - Dump Registers Command

static int __command_dump_registers(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc != 1) {
		debugger_print(debugger, "print_registers - Print the contents of the emulator's registers\n"
				"This command will print the contents of the registers of the emulator\n"
				" at the time of running.\n");
		return 0;
	}

	z80_cpu_t cpu = &debugger->asic->cpu;

	z80_registers_t r = cpu->registers;
	debugger_print(debugger, "   AF: 0x%04X   BC: 0x%04X   DE: 0x%04X  HL: 0x%04X\n", r.AF, r.BC, r.DE, r.HL);
	debugger_print(debugger, "  'AF: 0x%04X  'BC: 0x%04X  'DE: 0x%04X 'HL: 0x%04X\n", r._AF, r._BC, r._DE, r._HL);
	debugger_print(debugger, "   PC: 0x%04X   SP: 0x%04X   IX: 0x%04X  IY: 0x%04X\n", r.PC, r.SP, r.IX, r.IY);
	debugger_print(debugger, "   IFF1: %d      IFF2: %d      IM %d\n", cpu->IFF1, cpu->IFF2, cpu->int_mode);
	debugger_print(debugger, "Flags: ");
	if (r.flags.S) debugger_print(debugger, "S ");
	if (r.flags.Z) debugger_print(debugger, "Z ");
	if (r.flags.H) debugger_print(debugger, "H ");
	if (r.flags.PV) debugger_print(debugger, "P/V ");
	if (r.flags.N) debugger_print(debugger, "N ");
	if (r.flags.C) debugger_print(debugger, "C ");
	if (r.F == 0) debugger_print(debugger, "None set");
	debugger_print(debugger, "\n");
	if (cpu->halted) debugger_print(debugger, "CPU halted\n");

	return 0;
}

const struct debugger_command DumpRegistersCommand = {
	.name = "print_registers",
	.summary = "Dumps the contents of the registers.",
	.callback = __command_dump_registers,
};

//MARK: - Dump Stack Command

static int __command_dump_stack(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc > 1) {
		debugger_print(debugger, "%s [count] - print first `count` (or 10) items on the stack\n", argv[0]);
		return 0;
	}

	z80_cpu_t cpu = &debugger->asic->cpu;
	uint16_t sp = cpu->registers.SP;

	for (uint16_t i = sp; i != (uint16_t)(sp + 20); i += 2)
		debugger_print(debugger, "0x%04X: 0x%04X\n", i, cpu_read_word(cpu, i));

	return 0;
}

const struct debugger_command DumpStackCommand = {
	.name = "stack",
	.usage = "[count]",
	.summary = "Dumps the contents of the stack.",
	.callback = __command_dump_stack,
};
