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
#include <z80e/devices/keyboard.h>
#include <z80e/devices/link.h>
#include <z80e/devices/mapping.h>
#include <z80e/hardware/interrupts.h>
#include <z80e/hardware/mmu.h>
#include <z80e/hardware/t6a04.h>
#include <z80e/ti/runloop.h>
#include <z80e/ti/ti.h>

#ifndef NOLINK
#include <poll.h>
#endif

typedef enum {
	BATTERIES_REMOVED,
	BATTERIES_LOW,
	BATTERIES_GOOD
} battery_state;

struct asic {
	int stopped;
	ti_device_type device;
	battery_state battery;
	int battery_remove_check;
	int clock_rate;

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

	struct {
		struct hooks_execution on_before_execution;
		struct hooks_execution on_after_execution;
		struct hooks_lcd on_lcd_update;
	} hook;

	debugger_t *debugger;
};

int asic_set_clock_rate(asic_t , int);

int asic_add_timer(asic_t , int, double, timer_callback_t, void *);
void asic_remove_timer(asic_t , int);

//MARK: - Lifecycle Management

void asic_init(asic_t asic, ti_device_type);
void asic_deinit(asic_t asic);

//MARK: - Runloop Management

void asic_tick(asic_t asic);

void asic_tick_cycles(asic_t asic, int cycles);

//MARK: - Port Management

/// Installs the provided device onto the chip at the specified port.
/// - Parameters:
///   - asic: The chip.
///   - device: The device to install.
///   - port: The port on which to install.
void asic_install(asic_t asic, const device_t device, unsigned char port);

/// Retrieves the device installed on the specified port.
/// - Parameters:
///   - asic: The chip.
///   - port: The port at which to retrieve the device.
device_t asic_device(asic_t asic, unsigned char port);

//MARK: - Interrupts Management

void __asic_interrupt_update(asic_t asic);

/// Trigger an interrupt on the chip.
/// - Parameters:
///   - asic: The chip.
///   - interrupt: The interrupts to trigger.
void asic_interrupt(asic_t asic, enum ti_interrupt interrupt);

void asic_allow(asic_t asic, enum ti_interrupt interrupt, bool enabled);

void asic_acknowledge(asic_t asic, enum ti_interrupt interrupt);

//MARK: - Control Management

void asic_power_press(asic_t asic);
void asic_power_release(asic_t asic);

void asic_timer1_frequency(asic_t asic, double speed);
void asic_timer2_frequency(asic_t asic, double speed);
