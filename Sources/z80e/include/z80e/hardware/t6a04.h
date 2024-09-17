#pragma once

#include <z80e/types.h>
#include <z80e/debugging/hooks.h>

#include <stdint.h>

struct ti_bw_lcd {

	asic_t asic;

	/// [X * 64 + Y]
	uint8_t *ram;

	struct {
		struct hooks_lcd update;
	} hook;

	/// Which is up-down.
	int X;
	/// Which is left-right.
	int Y;
	/// Which is which y is rendered at top.
	int Z;
	/// On the hardware, reads are buffered in a register.
	uint8_t read_reg;
	/// 0-63
	uint8_t contrast;

	/// set=up, unset=down
	uint8_t up: 1;
	/// set=Y, unset=X
	uint8_t counter: 1;
	/// set=8, unset=6
	uint8_t word_length: 1;
	/// set=on, unset=off
	uint8_t display_on: 1;
	/// 0-3
	uint8_t op_amp1: 2;
	/// 0-3
	uint8_t op_amp2: 2;
};

void setup_lcd_display(asic_t );

uint8_t bw_lcd_read_screen(ti_bw_lcd_t *, int, int);
int bw_lcd_write_screen(ti_bw_lcd_t *, int, int, char);

void bw_lcd_reset(ti_bw_lcd_t *);

uint8_t bw_lcd_status_read(device_t device);
void bw_lcd_status_write(device_t device, uint8_t);

uint8_t bw_lcd_data_read(device_t device);
void bw_lcd_data_write(device_t device, uint8_t);

void bw_lcd_advance_int_pointer(ti_bw_lcd_t *, int *, int *);
void bw_lcd_advance_pointer(ti_bw_lcd_t *);
