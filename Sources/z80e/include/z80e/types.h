//
//  types.h
//  z80e
//
//  Created by Christophe Bronner on 2024-03-08.
//

#pragma once

//MARK: - Hardware

typedef struct asic asic_t;

typedef struct z80_cpu z80cpu_t;

typedef struct z80_regstate z80registers_t;

typedef struct ti_interrupts ti_interrupts_t;

typedef struct ti_bw_lcd ti_bw_lcd_t;

//MARK: - Devices

typedef struct z80_device *device_t;

//TODO: pass the asic in device functions
//typedef unsigned char (* _Nullable read_t)(asic_t * _Nonnull , void * _Null_unspecified);
//typedef void (* _Nullable write_t)(asic_t * _Nonnull, void * _Null_unspecified, unsigned char);

typedef unsigned char (* _Nullable read_t)(const device_t _Nonnull);

typedef void (* _Nullable write_t)(const device_t _Nonnull, unsigned char);
