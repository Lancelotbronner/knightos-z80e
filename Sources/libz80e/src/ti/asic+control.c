//
//  asic+control.c
//  z80e
//
//  Created by Christophe Bronner on 2024-09-13.
//

#include <z80e/ti/asic.h>

//MARK: - Power Button

void asic_power_press(asic_t asic) {
	asic->interrupts.interrupted.power_key = false;
	asic_interrupt(asic, INTERRUPT_POWER_KEY);
}

void asic_power_release(asic_t asic) {
	asic->interrupts.interrupted.power_key = true;
}

//MARK: - Interrupt Timers

static void __timer1_tick(asic_t asic, void *peripheral) {
	asic_interrupt(asic, INTERRUPT_FIRST_TIMER);
}

static void __timer2_tick(asic_t asic, void *peripheral) {
	asic_interrupt(asic, INTERRUPT_SECOND_TIMER);
}

void asic_timer1_frequency(asic_t asic, double speed) {
	if (asic->interrupts.timer1 != -1) {
		asic_remove_timer(asic, asic->interrupts.timer1);
		asic->interrupts.timer1 = -1;
	}

	if (speed != 0)
		asic->interrupts.timer1 = asic_add_timer(asic, 0, speed, __timer1_tick, nullptr);
}

void asic_timer2_frequency(asic_t asic, double speed) {
	if (asic->interrupts.timer2 != -1) {
		asic_remove_timer(asic, asic->interrupts.timer2);
		asic->interrupts.timer2 = -1;
	}

	if (speed != 0)
		asic->interrupts.timer2 = asic_add_timer(asic, 0, speed, __timer2_tick, nullptr);
}
