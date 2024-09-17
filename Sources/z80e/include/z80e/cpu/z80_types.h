#pragma once

#include <z80e/types.h>

#include <stdint.h>

enum z80_flags : uint8_t {
	FLAG_S =  0x80,
	FLAG_Z =  0x40,
	FLAG_5 =  0x20,
	FLAG_H =  0x10,
	FLAG_3 =  0x8,
	FLAG_PV = 0x4,
	FLAG_N  = 0x2,
	FLAG_C  = 0x1,
	FLAG_NONE = 0x0,
};

enum z80_registers : uint32_t {
	A = 0x1,
	F = 0x2,
	AF = 0x4,
	_AF = 0x8,
	B = 0x10,
	C = 0x20,
	BC = 0x40,
	_BC = 0x80,

	D = 0x0100,
	E = 0x0200,
	DE = 0x0400,
	_DE = 0x0800,
	H = 0x1000,
	L = 0x2000,
	HL = 0x4000,
	_HL = 0x8000,

	PC = 0x010000,
	SP = 0x020000,
	I = 0x040000,
	R = 0x080000,
	IXH = 0x100000,
	IXL = 0x200000,
	IX = 0x400000,
	IYH = 0x800000,

	IYL = 0x01000000,
	IY = 0x02000000,
};

struct z80_regstate {
	union {
		uint16_t AF;
		struct {
			union {
				uint8_t F;
				struct {
					uint8_t C  : 1;
					uint8_t N  : 1;
					uint8_t PV : 1;
					uint8_t _3 : 1;
					uint8_t H  : 1;
					uint8_t _5 : 1;
					uint8_t Z  : 1;
					uint8_t S  : 1;
				} flags;
			};
			uint8_t A;
		};
	};
	union {
		uint16_t BC;
		struct {
			uint8_t C;
			uint8_t B;
		};
	};
	union {
		uint16_t DE;
		struct {
			uint8_t E;
			uint8_t D;
		};
	};
	union {
		uint16_t HL;
		struct {
			uint8_t L;
			uint8_t H;
		};
	};
	uint16_t _AF, _BC, _DE, _HL;
	uint16_t PC, SP;
	union {
		uint16_t IX;
		struct {
			uint8_t IXL;
			uint8_t IXH;
		};
	};
	union {
		uint16_t IY;
		struct {
			uint8_t IYL;
			uint8_t IYH;
		};
	};
	uint8_t I, R;
	uint16_t WZ;
};
