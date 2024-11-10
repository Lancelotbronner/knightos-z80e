#pragma once

#include <z80e/types.h>
#include <z80e/debugging/hooks.h>

#include <stdint.h>

#define LCD_T6A04_RAM 7680 // 64 * 120

struct lcd_t6a04 {
	/// [X * 64 + Y]
	uint8_t ram[LCD_T6A04_RAM];

	struct {
		struct hooks_lcd update;
	} hook;

	/// Which is up-down. 6-bit
	int8_t X;
	/// Which is left-right. 7-bit
	int8_t Y;
	/// Which is which y is rendered at top.
	int8_t Z;
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

void lcd_t6a04_init(lcd_t6a04_t lcd);

void lcd_t6a04_clear(lcd_t6a04_t lcd);
void lcd_t6a04_reset(lcd_t6a04_t lcd);

bool lcd_t6a04_read(lcd_t6a04_t lcd, int x, int y);
bool lcd_t6a04_write(lcd_t6a04_t lcd, int x, int y, bool value);
