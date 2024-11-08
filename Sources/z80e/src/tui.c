#include <z80e/tui.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

static int tui_vprint(debugger_t debugger, const char *format, va_list args) {
	return vprintf(format, args);
}

static void tui_open(debugger_t debugger, const char *command) {

}

static void tui_close(debugger_t debugger) {

}

static void tui_deinit(debugger_t debugger) {

}

void tui_init(debugger_t debugger) {
	debugger->vprint = tui_vprint;
	debugger->open = tui_open;
	debugger->close = tui_close;
	debugger->deinit = tui_deinit;
	fprintf(stderr, "Sourcing z80erc...\n");
	debugger_source(debugger, "z80erc");
}

struct tui_disasm {
	ti_mmu_t mmu;
	char *string_pointer;
};

uint8_t tui_disassemble_read(struct disassemble_memory *state, uint16_t mem) {
	struct tui_disasm *disasm = state->extra_data;
	return ti_read_byte(disasm->mmu, mem);
}

int tui_disassemble_write(struct disassemble_memory *state, const char *format, ...) {
	struct tui_disasm *disasm = state->extra_data;
	va_list list;
	va_start(list, format);
	int count = vsprintf(disasm->string_pointer, format, list);
	disasm->string_pointer += count;
	return count;
}

void tui_tick(debugger_t debugger) {
	asic_t asic = debugger->asic;
	struct tui_disasm disasm_custom = { &asic->mmu, 0 };
	struct disassemble_memory disasm = { tui_disassemble_read, 0, &disasm_custom };

	while (true) {
		char prompt_buffer[80];
		char *current_pointer = prompt_buffer;
		mmu_bank_t bank = mmu_bank(&asic->mmu, asic->cpu.registers.PC / 0x4000);
		current_pointer += sprintf(prompt_buffer, "z80e [%c:%02X:0x%04X ", bank.flash ? 'F' : 'R', bank.page, asic->cpu.registers.PC);

		disasm_custom.string_pointer = current_pointer;
		disasm.current = asic->cpu.registers.PC;
		parse_instruction(&disasm, tui_disassemble_write, debugger->flags.knightos);
		current_pointer = disasm_custom.string_pointer;

		sprintf(current_pointer, "] %s> ", asic->cpu.halted ? "HALT " : "");
		char *result = readline(prompt_buffer);

		if (!result)
			break;

		int from_history = 0;
		
		if (*result == 0) {
			HIST_ENTRY *hist = history_get(where_history());
			if (hist == 0) {
				free(result);
				continue;
			}
			result = (char *)hist->line;
			from_history = 1;
		}
		if (strcmp(result, "exit") == 0)
			break;

		add_history(result);

		debugger->open(debugger, result);

		int retval = debugger_execute(debugger, result);
		if (retval > 0) {
			printf("The command returned %d\n", retval);
		}

		debugger->close(debugger);

		if (!from_history)
			free(result);
	}
}
