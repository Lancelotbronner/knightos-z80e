#include <z80e/debugging/command.h>
#include <z80e/debugging/debugger.h>

#include <z80e/ti/asic.h>

#include <ctype.h>
#include <string.h>

//MARK: - Dump Mappings Command

int __command_dump_mappings(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc > 2 || (argc == 2 && strcmp(argv[1], "-v") != 0)) {
		debugger_print(debugger, "%s [-v] - Print the MMU page mappings.\n The -v flag adds verbosity to the output.\n", argv[0]);
		debugger_print(debugger, " The terse output is formatted like this: \"Bank 0: F:00\".\n"
				" 'F' says the mapped page is a flash page, 'R' says the mapped page is a ram page.\n");
		return 0;
	}

	int verbose = argc > 1 && strcmp(argv[1], "-v") == 0;
	int i = 0;

	for (i = 0; i < 4; i++) {
		mmu_bank_t mapping = mmu_bank(&debugger->asic->mmu, i);
		if (verbose)
			debugger_print(debugger, "Page %d (0x%04X - 0x%04X): mapped to %s page %d (0x%04X - 0x%04X).\n",
				i, i * 0x4000, (i + 1) * 0x4000 - 1, mapping.flash ? "ROM" : "RAM", mapping.page,
				mapping.page * 0x4000, (mapping.page + 1) * 0x4000 - 1);
		else
			debugger_print(debugger, "Bank %d: %c:%02X\n", i, mapping.flash ? 'F' : 'R', mapping.page);
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

static int __command_hexdump(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc > 3) {
		debugger_print(debugger, "%s [start] [length] - print an amount of bytes from the memory\n"
				" Prints the bytes starting from `start` (or PC), `length` (default: 64) bytes in total.\n", argv[0]);
		return 0;
	}

	z80_cpu_t cpu = &debugger->asic->cpu;

	uint16_t start = debugger->asic->cpu.registers.PC;
	if (argc > 1)
		start = debugger_evaluate(debugger, argv[1]);

	uint16_t length = 64;
	if (argc > 2)
		length = debugger_evaluate(debugger, argv[2]);

	uint16_t i, total = 0;

	while (length > 0) {
		debugger_print(debugger, "0x%04X ", start);

		for (i = 0; i < 8 && length - i > 0; i++) {
			debugger_print(debugger, "%02X ", cpu->memory_read(cpu->memory, start + i));
		}
		start += i;
		length -= i;
		total += i;

		debugger_print(debugger, " ");

		for (i = 0; i < 8 && length - i > 0; i++) {
			debugger_print(debugger, "%02X ", cpu->memory_read(cpu->memory, start + i));
		}
		start += i;
		length -= i;
		total += i;

		debugger_print(debugger, "%*s|", (16 - total) * 3 + (i < 8 ? 1 : 0), " ");
		for (i = 0; i < total; i++) {
			char c = cpu->memory_read(cpu->memory, start - 16 + i);
			if (isprint(c) && c != '\t') {
				debugger_print(debugger, "%c", c);
			} else {
				debugger_print(debugger, ".");
			}
		}
		debugger_print(debugger, "|\n");
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

static int __command_backwards_hexdump(debugger_t debugger, int argc, char **argv) {
	if (argc > 3) {
		debugger_print(debugger, "%s [start] [length] - print an amount of bytes from the memory\n"
				" Prints the bytes starting from `start` (or PC), `length` (default: 64) bytes in total.\n", argv[0]);
		return 0;
	}

	z80_cpu_t cpu = &debugger->asic->cpu;

	uint16_t start = debugger->asic->cpu.registers.PC;
	if (argc > 1)
		start = debugger_evaluate(debugger, argv[1]);

	uint16_t length = 64;
	if (argc > 2)
		length = debugger_evaluate(debugger, argv[2]);

	uint16_t i, total = 0;

	while (length > 0) {
		debugger_print(debugger, "0x%04X ", start);

		for (i = 0; i < 8 && length - i > 0; i++) {
			debugger_print(debugger, "%02X ", cpu->memory_read(cpu->memory, start - i));
		}
		start -= i;
		length -= i;
		total += i;

		debugger_print(debugger, " ");

		for (i = 0; i < 8 && length - i > 0; i++)
			debugger_print(debugger, "%02X ", cpu->memory_read(cpu->memory, start + i));
		start -= i;
		length -= i;
		total += i;

		debugger_print(debugger, "%*s|", (16 - total) * 3 + (i < 8 ? 1 : 0), " ");
		for (i = 0; i < total; i++) {
			char c = cpu->memory_read(cpu->memory, start - 16 + i);
			if (isprint(c) && c != '\t')
				debugger_print(debugger, "%c", c);
			else
				debugger_print(debugger, ".");
		}
		debugger_print(debugger, "|\n");
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
