//
//  timer.c
//  z80e
//
//  Created by Christophe Bronner on 2024-09-12.
//

#include <z80e/hardware/timer.h>

bool timer_available(z80_timer_t timer) {
	return !timer->callback;
}
