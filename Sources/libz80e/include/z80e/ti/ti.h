//
//  ti/ti.h
//  z80e
//
//  Created by Christophe Bronner on 2024-01-01.
//

#pragma once

/*
 Z80-based graphing:
 TI-73, TI-81, TI-82, TI-83x, TI-84x, TI-85, TI-86

 M68k-based graphing
 TI-89, TI-92x, Voyage 200 PLT

 ARM-based graphing
 TI-Nspire, TI-Nspire CAS
 */

typedef enum {
	TI73,
	TI83p,
	TI83pSE,
	TI84p,
	TI84pSE,
	TI84pCSE,
	TI84pCE,
} ti_device_type;
