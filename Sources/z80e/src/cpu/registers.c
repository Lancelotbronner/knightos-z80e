#include <stdio.h>
#include <stdint.h>
#include <z80e/cpu/z80.h>

int parity(uint8_t x) {
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return x & 1;
}

void print_state(z80_cpu_t cpu) {
	struct z80_regstate *r = &cpu->registers;
	printf("   AF: 0x%04X   BC: 0x%04X   DE: 0x%04X  HL: 0x%04X\n", r->AF, r->BC, r->DE, r->HL);
	printf("  'AF: 0x%04X  'BC: 0x%04X  'DE: 0x%04X 'HL: 0x%04X\n", r->_AF, r->_BC, r->_DE, r->_HL);
	printf("   PC: 0x%04X   SP: 0x%04X   IX: 0x%04X  IY: 0x%04X\n", r->PC, r->SP, r->IX, r->IY);
	printf("Flags: ");
	if (r->flags.S) printf("S ");
	if (r->flags.Z) printf("Z ");
	if (r->flags.H) printf("H ");
	if (r->flags._3) printf("5 ");
	if (r->flags.PV) printf("P/V ");
	if (r->flags._5) printf("3 ");
	if (r->flags.N) printf("N ");
	if (r->flags.C) printf("C ");
	if (r->F == 0) printf("None set");
	printf("\n");
}
