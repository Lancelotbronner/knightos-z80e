//
//  ti/hardware/timer.h
//  z80e
//
//  Created by Christophe Bronner on 2024-09-12.
//

#pragma once

#include <z80e/types.h>

struct z80_hardware_timer {
	int cycles_until_tick;
	double frequency;
	timer_callback_t callback;
	void *data;
};

bool timer_available(z80_timer_t timer);
