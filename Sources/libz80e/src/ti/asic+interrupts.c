//
//  asic+interrupts.c
//  z80e
//
//  Created by Christophe Bronner on 2024-09-12.
//

#include <z80e/log.h>
#include <z80e/ti/asic.h>

void __asic_interrupt_update(asic_t asic) {
	if (asic->interrupts.interrupted.flags)
		return;
	z80e_debug("interrupts", "disabling interrupt pin");
	asic->cpu.interrupt = 0;
}

void asic_interrupt(asic_t asic, enum ti_interrupt interrupt) {
	// Trigger requested interrupts if they're enabled
	interrupt &= asic->interrupts.enabled.flags;
	asic->interrupts.interrupted.flags |= interrupt;

	if (!interrupt)
		return;

	// Flag our interruption
	asic->cpu.interrupt = true;

	//TODO: Find a better way to log interrupts
	union ti_interrupt_flags logged = asic->interrupts.interrupted;

	if (logged.power_key)
		z80e_debug("interrupts", "Triggered ON interrupt");
	if (logged.first_timer)
		z80e_debug("interrupts", "Triggered first timer interrupt");
	if (logged.second_timer)
		z80e_debug("interrupts", "Triggered second timer interrupt");
	if (logged.link_activity)
		z80e_debug("interrupts", "Triggered link activity interrupt");
	if (logged.first_crystal)
		z80e_debug("interrupts", "Triggered first crystal interrupt");
	if (logged.second_crystal)
		z80e_debug("interrupts", "Triggered second crystal interrupt");
	if (logged.third_crystal)
		z80e_debug("interrupts", "Triggered third crystal interrupt");
}

void asic_allow(asic_t asic, enum ti_interrupt interrupt, bool enabled) {
	if (enabled)
		asic->interrupts.enabled.flags |= interrupt;
	else
		asic->interrupts.enabled.flags &= ~interrupt;

	//TODO: Find a better way to log interrupts
	union ti_interrupt_flags logged = { .flags = interrupt };
	const char *arg = enabled ? "enabled" : "disabled";

	if (logged.power_key)
		z80e_debug("interrupts", "on key interrupt %s", arg);
	if (logged.first_timer)
		z80e_debug("interrupts", "first timer interrupt %s", arg);
	if (logged.second_timer)
		z80e_debug("interrupts", "second timer interrupt %s", arg);
	if (logged.link_activity)
		z80e_debug("interrupts", "link activity interrupt %s", arg);
	if (logged.first_crystal)
		z80e_debug("interrupts", "first crystal interrupt %s", arg);
	if (logged.second_crystal)
		z80e_debug("interrupts", "second crystal interrupt %s", arg);
	if (logged.third_crystal)
		z80e_debug("interrupts", "third crystal interrupt %s", arg);
}

void asic_acknowledge(asic_t asic, enum ti_interrupt interrupt) {
	asic->interrupts.interrupted.flags &= ~interrupt;

	//TODO: Find a better way to log interrupts
	union ti_interrupt_flags logged = { .flags = interrupt };

	if (logged.power_key)
		z80e_debug("interrupts", "on key interrupt acknowledged");
	if (logged.first_timer)
		z80e_debug("interrupts", "first timer interrupt acknowledged");
	if (logged.second_timer)
		z80e_debug("interrupts", "second timer interrupt acknowledged");
	if (logged.link_activity)
		z80e_debug("interrupts", "link activity interrupt acknowledged");
	if (logged.first_crystal)
		z80e_debug("interrupts", "first crystal interrupt acknowledged");
	if (logged.second_crystal)
		z80e_debug("interrupts", "second crystal interrupt acknowledged");
	if (logged.third_crystal)
		z80e_debug("interrupts", "third crystal interrupt acknowledged");

	__asic_interrupt_update(asic);
}
