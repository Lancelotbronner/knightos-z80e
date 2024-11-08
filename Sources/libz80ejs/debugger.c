#include <z80e/debugging/debugger.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*openti_js_print_string)(int, const char *);
typedef void (*openti_js_window_closed)(int);
typedef int (*openti_js_new_state)(int, debugger_t , const char *);

typedef struct {
	int js_reference;
	openti_js_print_string print_string;
	openti_js_new_state new_state;
	openti_js_window_closed window_closed;
} openti_interface_state;

char openti_print_buffer[256];

int openti_state_vprint(debugger_t debugger, const char *format, va_list list) {
	openti_interface_state *istate = debugger->data;
	int count = vsnprintf(openti_print_buffer, 256, format, list);
	istate->print_string(istate->js_reference, openti_print_buffer);
	return count;
}

void openti_close_window(debugger_t debugger) {
	openti_interface_state *istate = debugger->data;
	istate->window_closed(istate->js_reference);
	free(debugger->data);
	free(debugger);
}

debugger_t openti_new_state(debugger_t debugger, int ref) {
	debugger_t state = calloc(sizeof(debugger_t), 1);
	*state = *debugger;
	state->vprint = openti_state_vprint;
	state->deinit = openti_close_window;

	openti_interface_state *istate = state->data = calloc(sizeof(openti_interface_state), 1);
	istate->js_reference = ref;

	state->data = istate;
	return state;
}
