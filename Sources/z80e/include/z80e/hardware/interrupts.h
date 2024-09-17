//
//  hardware/interrupts.h
//  z80e
//
//  Created by Christophe Bronner on 2024-09-13.
//

#pragma once

#include <z80e/types.h>

enum ti_interrupt : unsigned char {
	INTERRUPT_POWER_KEY = 0x1,
	INTERRUPT_FIRST_TIMER = 0x2,
	INTERRUPT_SECOND_TIMER = 0x4,
	INTERRUPT_LOW_POWER_MODE = 0x8,
	INTERRUPT_LINK_ACTIVITY = 0x10,
	INTERRUPT_FIRST_CRYSTAL = 0x20,
	INTERRUPT_SECOND_CRYSTAL = 0x40,
	INTERRUPT_THIRD_CRYSTAL = 0x80,
};

//TODO: Review these interrupt flags, different devices may be returning different orders?
union ti_interrupt_flags {
	enum ti_interrupt flags;
	struct {
		bool power_key : 1;
		bool first_timer : 1;
		bool second_timer : 1;
		/// may be the same as `on_key_pressed`, or its a typo?
		bool low_power_mode : 1;
		bool link_activity : 1;
		bool first_crystal : 1;
		bool second_crystal : 1;
		bool third_crystal : 1;
	};
};

struct ti_interrupts {
	//TODO: Inline those timers?
	int timer1;
	int timer2;
	union ti_interrupt_flags interrupted;
	union ti_interrupt_flags enabled;
};
