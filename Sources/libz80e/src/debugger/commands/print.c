#include <z80e/debugging/commands.h>
#include <z80e/debugging/debugger.h>

#include <stdlib.h>
#include <string.h>

//MARK: - Print Command

static int __command_print(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc == 1) {
		debugger_print(debugger, "%s `expression` - Print an expression\n Don't forget to quote your expression!\n", argv[0]);
		return 0;
	}

	int strl = 0;
	int i;
	for (i = 0; i < argc - 1; i++) {
		strl += strlen(argv[i + 1]) + 1;
	}

	char *tmp = malloc(strl);
	char *dpointer = tmp;
	for (i = 0; i < argc - 1; i++) {
		strcpy(dpointer, argv[i + 1]);
		dpointer += strlen(argv[i + 1]);
		*(dpointer++) = ' ';
	}
	*(dpointer - 1) = 0;

	uint16_t expr = debugger_evaluate(debugger, tmp);

	debugger_print(debugger, "%s = 0x%04X (%u)\n", tmp, expr, expr);

	free(tmp);
	return 0;
}

const struct debugger_command PrintCommand = {
	.name = "print",
	.usage = "<expression>",
	.summary = "Evaluates an expression",
	.callback = __command_print,
};
