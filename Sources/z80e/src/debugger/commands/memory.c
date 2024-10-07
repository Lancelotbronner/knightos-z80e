#include <z80e/debugging/command.h>
#include <z80e/debugging/debugger.h>

#include <z80e/ti/asic.h>

#include <ctype.h>
#include <string.h>

//MARK: - Dump Mappings Command

int __command_dump_mappings(struct debugger_state *state, void *data, int argc, char **argv) {
	if (argc > 2 || (argc == 2 && strcmp(argv[1], "-v") != 0)) {
		state->print(state, "%s [-v] - Print the MMU page mappings.\n The -v flag adds verbosity to the output.\n", argv[0]);
		state->print(state, " The terse output is formatted like this: \"Bank 0: F:00\".\n"
				" 'F' says the mapped page is a flash page, 'R' says the mapped page is a ram page.\n");
		return 0;
	}

	int verbose = argc > 1 && strcmp(argv[1], "-v") == 0;
	int i = 0;

	for (i = 0; i < 4; i++) {
		mmu_bank_t mapping = mmu_bank(&state->asic->mmu, i);
		if (verbose)
			state->print(state, "Page %d (0x%04X - 0x%04X): mapped to %s page %d (0x%04X - 0x%04X).\n",
				i, i * 0x4000, (i + 1) * 0x4000 - 1, mapping.flash ? "ROM" : "RAM", mapping.page,
				mapping.page * 0x4000, (mapping.page + 1) * 0x4000 - 1);
		else
			state->print(state, "Bank %d: %c:%02X\n", i, mapping.flash ? 'F' : 'R', mapping.page);
	}

	return 0;
}

const struct debugger_command DumpMappingsCommand = {
	.name = "mappings",
	.summary = "Print the MMU page mappings",
	.description =
	"The -v flag adds verbosity to the output." "\n"
	"\n"
	"The terse output is formatted like this:" "\n"
	"    Bank 0: F:00" "\n"
	"'F' indicates a flash page while 'R' indicates a ram page.",
	.callback = __command_dump_mappings,
};

//MARK: - Hexdump Command

static int __command_hexdump(struct debugger_state *state, void *data, int argc, char **argv) {
	if (argc > 3) {
		state->print(state, "%s [start] [length] - print an amount of bytes from the memory\n"
				" Prints the bytes starting from `start` (or PC), `length` (default: 64) bytes in total.\n", argv[0]);
		return 0;
	}

	z80_cpu_t cpu = &state->asic->cpu;

	uint16_t start = state->asic->cpu.registers.PC;
	if (argc > 1) {
		start  = debugger_evaluate(state, argv[1]);
	}

	uint16_t length = 64;
	if (argc > 2) {
		length = debugger_evaluate(state, argv[2]);
	}

	uint16_t i, total = 0;

	while (length > 0) {
		state->print(state, "0x%04X ", start);

		for (i = 0; i < 8 && length - i > 0; i++) {
			state->print(state, "%02X ", cpu->read_byte(cpu->memory, start + i));
		}
		start += i;
		length -= i;
		total += i;

		state->print(state, " ");

		for (i = 0; i < 8 && length - i > 0; i++) {
			state->print(state, "%02X ", cpu->read_byte(cpu->memory, start + i));
		}
		start += i;
		length -= i;
		total += i;

		state->print(state, "%*s|", (16 - total) * 3 + (i < 8 ? 1 : 0), " ");
		for (i = 0; i < total; i++) {
			char c = cpu->read_byte(cpu->memory, start - 16 + i);
			if (isprint(c) && c != '\t') {
				state->print(state, "%c", c);
			} else {
				state->print(state, ".");
			}
		}
		state->print(state, "|\n");
		total = 0;
	}

	return 0;
}

const struct debugger_command HexdumpCommand = {
	.name = "dump",
	.usage = "[start] [length]",
	.summary = "Dumps memory",
	.description =
	"`start` defaults to PC." "\n"
	"`length` defaults to 64." "\n"
	"\n"
	"Prints `length` bytes starting from `start`." "\n",
	.callback = __command_hexdump,
};

//MARK: - Backwards Dump Command

static int __command_backwards_hexdump(struct debugger_state *state, int argc, char **argv) {
	if (argc > 3) {
		state->print(state, "%s [start] [length] - print an amount of bytes from the memory\n"
				" Prints the bytes starting from `start` (or PC), `length` (default: 64) bytes in total.\n", argv[0]);
		return 0;
	}

	z80_cpu_t cpu = &state->asic->cpu;

	uint16_t start = state->asic->cpu.registers.PC;
	if (argc > 1) {
		start  = debugger_evaluate(state, argv[1]);
	}

	uint16_t length = 64;
	if (argc > 2) {
		length = debugger_evaluate(state, argv[2]);
	}

	uint16_t i, total = 0;

	while (length > 0) {
		state->print(state, "0x%04X ", start);

		for (i = 0; i < 8 && length - i > 0; i++) {
			state->print(state, "%02X ", cpu->read_byte(cpu->memory, start - i));
		}
		start -= i;
		length -= i;
		total += i;

		state->print(state, " ");

		for (i = 0; i < 8 && length - i > 0; i++) {
			state->print(state, "%02X ", cpu->read_byte(cpu->memory, start + i));
		}
		start -= i;
		length -= i;
		total += i;

		state->print(state, "%*s|", (16 - total) * 3 + (i < 8 ? 1 : 0), " ");
		for (i = 0; i < total; i++) {
			char c = cpu->read_byte(cpu->memory, start - 16 + i);
			if (isprint(c) && c != '\t') {
				state->print(state, "%c", c);
			} else {
				state->print(state, ".");
			}
		}
		state->print(state, "|\n");
		total = 0;
	}

	return 0;
}

const struct debugger_command HexdumpBackwardsCommand = {
	.name = "bdump",
	.usage = "[start] [length]",
	.summary = "Dumps memory backwards",
	.description =
	"`start` defaults to PC." "\n"
	"`length` defaults to 64." "\n"
	"\n"
	"Prints `length` bytes from `start` backwards." "\n",
	.callback = __command_hexdump,
};
