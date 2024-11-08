//
//  app.h
//  z80e
//
//  Created by Christophe Bronner on 2024-11-08.
//

#pragma once

#include <z80e/ti/ti.h>

bool z80e_init();
void z80e_deinit();

asic_t z80e_asic();
debugger_t z80e_debugger();
void z80e_start();

void z80e_args(int argc, char *argv[]);
void z80e_help(bool exit);

bool z80e_hook_lcd(float refresh_rate, hook_lcd_callback_t callback);
void z80e_dirty_lcd();

void z80e_check_rom(bool enabled);
void z80e_cycles(int cycles);
void z80e_device(ti_device_type device);
void z80e_debugging(bool enabled);
void z80e_print_state(bool enabled);
void z80e_rom(char *filepath);
