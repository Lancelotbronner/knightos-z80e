#pragma once

#include <stdint.h>
#ifndef NOLINK
#include <poll.h>
#endif

#include <z80e/types.h>

#include <z80e/cpu/z80.h>
#include <z80e/debugger/hooks.h>
#include <z80e/debugger/debugger.h>
#include <z80e/devices/keyboard.h>
#include <z80e/devices/mapping.h>
#include <z80e/log/log.h>
#include <z80e/ti/runloop.h>
#include <z80e/ti/memory.h>
#include <z80e/ti/ti.h>
#include <z80e/hardware/interrupts.h>
#include <z80e/hardware/timer.h>

typedef enum {
	BATTERIES_REMOVED,
	BATTERIES_LOW,
	BATTERIES_GOOD
} battery_state;

struct z80_link_socket {
#ifndef NOLINK
	int accept;
	struct pollfd listenfd;
	struct pollfd clients[10];
#endif
};

struct asic {
	int stopped;
	ti_device_type device;
	battery_state battery;
	int battery_remove_check;
	int clock_rate;

	struct z80_cpu cpu;
	struct z80_runloop runloop;
	struct keyboard_device keyboard;
	struct mapping_device mapping;
	struct ti_mmu mmu;
	struct ti_interrupts interrupts;
	z80_link_socket_t *link;

	struct {
		int capacity;
		z80_timer_t head;
	} timers;

	hook_info_t *hook;
	log_t *log;
	debugger_t *debugger;
};

int asic_set_clock_rate(asic_t , int);

int asic_add_timer(asic_t , int, double, timer_callback_t, void *);
void asic_remove_timer(asic_t , int);

//MARK: - Lifecycle Management

void asic_init(asic_t asic, ti_device_type, log_t *);
void asic_deinit(asic_t asic);

//MARK: - Runloop Management

void asic_tick(asic_t asic);

void asic_tick_cycles(asic_t asic, int cycles);

//MARK: - Device Management

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
