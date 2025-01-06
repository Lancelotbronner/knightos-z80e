//
//  types.h
//  z80e
//
//  Created by Christophe Bronner on 2024-03-08.
//

#pragma once

// Only needed when imported into Swift tests
#include <stdbool.h>

//MARK: - Hardware

typedef struct asic *asic_t;

typedef struct z80_runloop *z80_runloop_t;

typedef struct z80_cpu *z80_cpu_t;

typedef struct ti_mmu *ti_mmu_t;

typedef struct z80_state z80_registers_t;

typedef struct ti_interrupts *ti_interrupts_t;

typedef struct lcd_t6a04 *lcd_t6a04_t;

typedef void (*timer_callback_t)(asic_t _Nonnull asic, void * _Nullable data);

typedef struct z80_hardware_timer *z80_timer_t;

typedef struct z80_link_socket *z80_link_t;

//MARK: - Peripheral Management

typedef struct peripheral *peripheral_t;

//TODO: pass the asic in peripheral functions?
//typedef unsigned char (* _Nullable read_t)(asic_t _Nonnull , void * _Null_unspecified);
//typedef void (* _Nullable write_t)(asic_t _Nonnull, void * _Null_unspecified, unsigned char);

typedef unsigned char (* _Nullable read_t)(const peripheral_t _Nonnull);

typedef void (* _Nullable write_t)(const peripheral_t _Nonnull, unsigned char);

//MARK: - Devices

typedef struct keyboard_device *keyboard_device_t;

typedef struct mapping_device *mapping_device_t;

typedef struct link_device *link_device_t;

//MARK: - Debugger

typedef struct debugger *debugger_t;

typedef struct debugger_command const *debugger_command_t;
