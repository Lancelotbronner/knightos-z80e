//
//  isa/z80.h
//  z80e
//
//  Created by Christophe Bronner on 2024-04-10.
//

#pragma once

struct z80i {
	union {
		unsigned char value;
		struct {
			unsigned _BitInt(2) x;
			unsigned _BitInt(3) y;
			unsigned _BitInt(3) z;
		};
		struct {
			unsigned _BitInt(2) : 2;
			unsigned _BitInt(2) p;
			unsigned _BitInt(1) q;
		};
	};
};

enum z80i_r : unsigned _BitInt(3) {
	R_B, R_C, R_D, R_E, R_H, R_L,
	/// (HL)
	R_HL,
	R_A
};

enum z80i_rp1 : unsigned _BitInt(2) {
	RP1_BC, RP1_DE, RP1_HL, RP1_SP,
};

enum z80i_rp2 : unsigned _BitInt(2) {
	RP2_BC, RP2_DE, RP2_HL, RP2_AF,
};

enum z80i_cc : unsigned _BitInt(3) {
	CC_NZ, CC_Z, CC_NC, CC_C, CC_PO, CC_PE, CC_P, CC_M
};

enum z80i_alu : unsigned _BitInt(3) {
	ALU_ADD, ALU_ADC, ALU_SUB, ALU_SBC, ALU_AND, ALU_XOR, ALU_OR, ALU_CP
};

enum z80i_rot : unsigned _BitInt(3) {
	ROT_RLC, ROT_RRC, ROT_RL, ROT_RR, ROT_SLA, ROT_SRA, ROT_SLL, ROT_SRL
};

enum z80i_im : unsigned _BitInt(2) {
	IM_0, IM_01, IM_1, IM_2,
};

enum z80i_bli : unsigned _BitInt(4) {
	LDI,	CPI,	INI,	OUTI,
	LDD,	CPD,	IND,	OUTD,
	LDIR,	CPIR,	INIR,	OTIR,
	LDDR,	CPDR,	INDR,	OTDR,
};
