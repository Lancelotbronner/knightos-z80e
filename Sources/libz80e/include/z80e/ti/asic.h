//
//  ti/asic.h
//  z80e
//
//  Created by Christophe Bronner on 2024-03-08.
//

#pragma once

#include <z80e/types.h>

#include <z80e/cpu/z80.h>
#include <z80e/debugging/debugger.h>
#include <z80e/peripherals/keyboard.h>
#include <z80e/peripherals/link.h>
#include <z80e/peripherals/mapping.h>
#include <z80e/hardware/interrupts.h>
#include <z80e/hardware/mmu.h>
#include <z80e/hardware/t6a04.h>
#include <z80e/ti/runloop.h>
#include <z80e/ti/ti.h>

#ifndef NOLINK
#include <poll.h>
#endif

enum battery_state {
	BATTERIES_REMOVED,
	BATTERIES_LOW,
	BATTERIES_GOOD
};

struct battery {
	enum battery_state state : 2;
	bool remove_check : 1;
};

struct asic {
	ti_device_type peripheral;
	int clock_rate;

	// TODO: Merge runloop, clock_rate and timers into new scheduler?
	// TODO: Merge socket into link?
	// TODO: Merge battery & battery_remove_check into new power?

	struct z80_cpu cpu;
	struct z80_runloop runloop;
	struct ti_mmu mmu;
	struct ti_interrupts interrupts;
	struct lcd_t6a04 lcd;
	struct mapping_device mapping;
	struct keyboard_device keyboard;
	struct link_device link;

#ifndef NOLINK
	struct {
		int accept;
		struct pollfd listenfd;
		struct pollfd clients[10];
	} socket;
#endif

	struct {
		int capacity;
		z80_timer_t head;
	} timers;

	struct battery battery;

	//TODO: move to debugger
	bool stopped : 1;
};

//MARK: - Lifecycle Management

void asic_init(asic_t asic, ti_device_type);
void asic_deinit(asic_t asic);

//MARK: - Scheduler Management

//TODO: Move these to scheduler

int asic_set_clock_rate(asic_t , int);

int asic_add_timer(asic_t , int, double, timer_callback_t, void *);
void asic_remove_timer(asic_t , int);

void asic_tick(asic_t asic);

void asic_tick_cycles(asic_t asic, int cycles);

//MARK: - Port Management

//TODO: Move these to z80?

/// Installs the provided peripheral onto the chip at the specified port.
/// - Parameters:
///   - asic: The chip.
///   - peripheral: The peripheral to install.
///   - port: The port on which to install.
void asic_install(asic_t asic, const peripheral_t peripheral, unsigned char port);

/// Retrieves the peripheral installed on the specified port.
/// - Parameters:
///   - asic: The chip.
///   - port: The port at which to retrieve the peripheral.
peripheral_t asic_device(asic_t asic, unsigned char port);

//MARK: - Interrupts Management

//TODO: Move these to z80?

void __asic_interrupt_update(asic_t asic);

/// Trigger an interrupt on the chip.
/// - Parameters:
///   - asic: The chip.
///   - interrupt: The interrupts to trigger.
void asic_interrupt(asic_t asic, enum ti_interrupt interrupt);

void asic_allow(asic_t asic, enum ti_interrupt interrupt, bool enabled);

void asic_acknowledge(asic_t asic, enum ti_interrupt interrupt);

//MARK: - Control Management

//TODO: Move these to interrupts

void asic_power_press(asic_t asic);
void asic_power_release(asic_t asic);

void asic_timer1_frequency(asic_t asic, double speed);
void asic_timer2_frequency(asic_t asic, double speed);
