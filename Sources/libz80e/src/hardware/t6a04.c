#include <z80e/types.h>

#include <z80e/ti/asic.h>
#include <z80e/hardware/t6a04.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <z80e/log.h>

void lcd_t6a04_init(lcd_t6a04_t lcd) {
	lcd_t6a04_reset(lcd);
}

void lcd_t6a04_reset(lcd_t6a04_t lcd) {
	lcd->word_length = 1;
	lcd->up = 1;
}

//MARK: - Status Management

union lcd_status {
	uint8_t value;
	struct {
		/// Whether it's busy (`1`) or can accept a command (`0`).
		bool busy : 1;
		/// Whether it's is 8-bit (`1`) or 6-bit (`0`) per word.
		bool byte : 1;
		/// Whether it's on (`1`) or off (`0`).
		bool on : 1;
		/// Whether it's in reset (`1`) or operating (`0`) state.
		bool reset : 1;
		// undefined
		bool : 1;
		bool : 1;
		/// Whether the counter is Y (`1`) or X (`0`).
		bool counter : 1;
		/// Whether the counter should increment (`1`) or decrement (`0`).
		bool inc : 1;
	};
};

uint8_t lcd_t6a04_status_read(peripheral_t peripheral) {
	lcd_t6a04_t lcd = peripheral->data;
	union lcd_status status = {};
	// always accepts commands
	// always in operating state
	status.byte = lcd->word_length;
	status.on = lcd->display_on;
	status.counter = lcd->counter;
	status.inc = lcd->up;
	return status.value;
}

void lcd_t6a04_status_write(peripheral_t peripheral, uint8_t val) {
	lcd_t6a04_t lcd = peripheral->data;

	z80e_debug("lcd", "Wrote 0x%02X (0b%b) to status", val, val);
	if ((val & 0xC0) == 0xC0) { // 0b11XXXXXX
		lcd->contrast = val & 0x3F;
		z80e_debug("lcd", "\tSet contrast to 0x%02X", lcd->contrast);
	} else if (val & 0x80) { // 0b10XXXXXX
		lcd->X = val & 0x3F;
		z80e_debug("lcd", "\tSet X (vertical!) to 0x%02X", lcd->X);
	} else if (val & 0x40) { // 0b01XXXXXX
		lcd->Z = val & 0x3F;
		z80e_debug("lcd", "\tSet Z (vertical scroll) to 0x%02X", lcd->Z);
	} else if (val & 0x20) { // 0b001XXXXX
		lcd->Y = val & 0x1F;
		z80e_debug("lcd", "\tSet Y (horizontal!) to 0x%02X", lcd->Y);
	} else if ((val & 0x18) == 0x18) { // 0b00011***
		// test mode - not emulating yet
		z80e_warning("lcd", "\tTest mode is dangerous and will damage your peripheral");
	} else if (val & 0x10) { // 0b00010*XX
		lcd->op_amp1 = val & 0x03;
		z80e_debug("lcd", "\tSet Op-Amp 1 to 0x%02X", lcd->op_amp1);
	} else if (val & 0x08) { // 0b00001*XX
		lcd->op_amp2 = val & 0x03;
		z80e_debug("lcd", "\tSet Op-Amp 2 to 0x%02X", lcd->op_amp2);
	} else if (val & 0x04) { // 0b000001XX
		lcd->counter = !!(val & 0x02);
		lcd->up = !!(val & 0x01);
		z80e_debug("lcd", "\tSet counter to %s and Up/Down to %s",
			lcd->counter ? "Y" : "X", lcd->up ? "Up" : "Down");
	} else if (val & 0x02) { // 0b0000001X
		lcd->display_on = !!(val & 0x01);
		z80e_debug("lcd", "\tDisplay turned %s", lcd->display_on ? "ON" : "OFF");
		hook_lcd_trigger(&lcd->hook.update, lcd);
	} else { // 0b0000000X
		lcd->word_length = !!(val & 0x01);
		z80e_debug("lcd", "\tWord Length set to %d", lcd->word_length ? 8 : 6);
	}
}

//MARK: - Pixel Management

static int __lcd_word(lcd_t6a04_t lcd) {
	return lcd->word_length ? 8 : 6;
}

bool lcd_t6a04_read(lcd_t6a04_t lcd, int y, int x) {
	int bit_size = __lcd_word(lcd);
	int location = x * 120 + y * bit_size;
	int byte_offset = location >> 3;
	int bit_offset = location % 8;

	uint8_t mask = 1 << bit_offset;
	uint8_t byte = lcd->ram[byte_offset];
	bool bit = byte & mask;
	return bit;
}

bool lcd_t6a04_write(lcd_t6a04_t lcd, int x, int y, bool value) {
	int bit_size = __lcd_word(lcd);
	int location = x * 120 + y * bit_size;
	int byte_offset = location >> 3;
	int bit_offset = location % 8;

	uint8_t mask = 1 << bit_offset;
	uint8_t *byte = lcd->ram[byte_offset];
	*byte &= ~mask;
	return !!byte;
}

//MARK: - Screen Management

static void __lcd_advance_coordinates(lcd_t6a04_t lcd, int *Y, int *X) {
	if (lcd->counter) { // Y
		(*Y)++;
		*Y %= 120;
	} else { // X
		(*X)++;
		*X %= 64;
	}
}

static void __lcd_advance_position(lcd_t6a04_t lcd) {
	int maxX = lcd->word_length ? 15 : 20;
	if (lcd->counter) { // Y
		if (!lcd->up) {
			lcd->Y--;
			if (lcd->Y < 0)
				lcd->Y = maxX - 1;
		} else {
			lcd->Y++;
			// wrap at maxX
			if (lcd->Y == maxX)
				lcd->Y = 0;
			// unless already out of range, then wrap at 32
			lcd->Y %= 32;
		}
	} else { // X
		if (!lcd->up) {
			lcd->X--;
			if (lcd->X < 0)
				lcd->X = 63;
		} else {
			lcd->X++;
			lcd->X %= 64;
		}
	}
}

static uint16_t __lcd_mask(lcd_t6a04_t lcd, int bit_offset) {
	return (lcd->word_length ? 0xFF : 0x3F) << (16 - bit_offset);
}

static uint16_t __lcd_shift(uint8_t value, int bit_offset, int bit_size) {
	uint16_t result = value;
	result <<= 16 - bit_offset - bit_size;
	return result;
}

static uint8_t __lcd_unshift(uint16_t value, int bit_offset, int bit_size) {
	value >>= 16 - bit_offset - bit_size;
	value &= ~(0xFFFF << bit_size);
	return value;
}

static uint8_t __lcd_screen_read(lcd_t6a04_t lcd) {
	int bit_size = __lcd_word(lcd);
	int location = lcd->X * 120 + lcd->Y * bit_size;
	int byte_offset = location >> 3;
	int bit_offset = location % 8;

	// fast path: aligned reads
	if (!bit_offset)
		return lcd->ram[byte_offset];

	// avoid alignment issues by using memcpy, use 16-bit values to cross byte boundaries.
	uint16_t value;
	void *destination = &lcd->ram[byte_offset];
	memcpy(&value, destination, sizeof(uint16_t));
	uint8_t result = __lcd_unshift(value, bit_offset, bit_size);

	return result;
}

static bool __lcd_screen_write(lcd_t6a04_t lcd, uint8_t value) {
	int bit_size = __lcd_word(lcd);
	int location = lcd->X * 120 + lcd->Y * bit_size;
	int byte_offset = location >> 3;
	int bit_offset = location % 8;
	uint8_t *destination = &lcd->ram[byte_offset];

	// fast path: aligned writes
	if (!bit_offset) {
		if (*destination == value)
			return false;
		*destination = value;
		return true;
	}

	// avoid alignment issues by using memcpy, use 16-bit values to cross byte boundaries.
	uint16_t result;
	memcpy(&result, destination, sizeof(uint16_t));
	uint16_t original = result;
	result &= ~__lcd_mask(lcd, bit_offset);
	result |= __lcd_shift(value, bit_offset, bit_size);

	if (original == result)
		return false;
	memcpy(destination, &result, sizeof(uint16_t));
	return true;
}

void lcd_t6a04_clear(lcd_t6a04_t lcd) {
	memset(lcd->ram, 0, LCD_T6A04_RAM);
}

//MARK: - Device Management (Status)

static unsigned char __status_read(peripheral_t peripheral) {
	lcd_t6a04_t lcd = peripheral->data;
	union lcd_status status = {};
	// always accepts commands
	// always in operating state
	status.byte = lcd->word_length;
	status.on = lcd->display_on;
	status.counter = lcd->counter;
	status.inc = lcd->up;
	return status.value;
}

static void __status_write(peripheral_t peripheral, unsigned char value) {
	lcd_t6a04_t lcd = peripheral->data;

	z80e_debug("lcd", "Wrote 0x%02X (0b%b) to status", value, value);
	if ((value & 0xC0) == 0xC0) { // 0b11XXXXXX
		lcd->contrast = value & 0x3F;
		z80e_debug("lcd", "\tSet contrast to 0x%02X", lcd->contrast);
	} else if (value & 0x80) { // 0b10XXXXXX
		lcd->X = value & 0x3F;
		z80e_debug("lcd", "\tSet X (vertical!) to 0x%02X", lcd->X);
	} else if (value & 0x40) { // 0b01XXXXXX
		lcd->Z = value & 0x3F;
		z80e_debug("lcd", "\tSet Z (vertical scroll) to 0x%02X", lcd->Z);
	} else if (value & 0x20) { // 0b001XXXXX
		lcd->Y = value & 0x1F;
		z80e_debug("lcd", "\tSet Y (horizontal!) to 0x%02X", lcd->Y);
	} else if ((value & 0x18) == 0x18) { // 0b00011***
		// test mode - not emulating yet
		z80e_warning("lcd", "\tTest mode is dangerous and will damage your peripheral");
	} else if (value & 0x10) { // 0b00010*XX
		lcd->op_amp1 = value & 0x03;
		z80e_debug("lcd", "\tSet Op-Amp 1 to 0x%02X", lcd->op_amp1);
	} else if (value & 0x08) { // 0b00001*XX
		lcd->op_amp2 = value & 0x03;
		z80e_debug("lcd", "\tSet Op-Amp 2 to 0x%02X", lcd->op_amp2);
	} else if (value & 0x04) { // 0b000001XX
		lcd->counter = !!(value & 0x02);
		lcd->up = !!(value & 0x01);
		z80e_debug("lcd", "\tSet counter to %s and Up/Down to %s",
			lcd->counter ? "Y" : "X", lcd->up ? "Up" : "Down");
	} else if (value & 0x02) { // 0b0000001X
		lcd->display_on = !!(value & 0x01);
		z80e_debug("lcd", "\tDisplay turned %s", lcd->display_on ? "ON" : "OFF");
		hook_lcd_trigger(&lcd->hook.update, lcd);
	} else { // 0b0000000X
		lcd->word_length = !!(value & 0x01);
		z80e_debug("lcd", "\tWord Length set to %d", lcd->word_length ? 8 : 6);
	}
}

void port_t6a04_status(peripheral_t peripheral, lcd_t6a04_t lcd) {
	peripheral->data = lcd;
	peripheral->read = __status_read;
	peripheral->write = __status_write;
}

//MARK: - Device Management (Data)

static unsigned char __data_read(peripheral_t peripheral) {
	lcd_t6a04_t lcd = peripheral->data;
	uint8_t result = lcd->read_reg;
	lcd->read_reg = __lcd_screen_read(lcd);
	__lcd_advance_position(lcd);
	return result;
}

static void __data_write(peripheral_t peripheral, unsigned char value) {
	lcd_t6a04_t lcd = peripheral->data;
	bool dirty = __lcd_screen_write(lcd, value);
	__lcd_advance_position(lcd);
	z80e_debug("lcd", "Wrote %02X (0b%b) to %d (Y), %d (X)", value, value, lcd->Y, lcd->X);
	if (dirty)
		hook_lcd_trigger(&lcd->hook.update, lcd);
}

void port_t6a04_data(peripheral_t peripheral, lcd_t6a04_t lcd) {
	peripheral->data = lcd;
	peripheral->read = __data_read;
	peripheral->write = __data_write;
}
