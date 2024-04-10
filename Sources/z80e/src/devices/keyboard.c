#include <z80e/devices/keyboard.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <z80e/ti/asic.h>
#include <z80e/core/cpu.h>

void keyboard_init(keyboard_t keyboard) {
	for (int i = 0; i < 8; i++)
		keyboard->groups[i] = 0xFF;
	keyboard->group_mask = 0;
}

//MARK: - Memory Management

keyboard_t keyboard_new() {
	keyboard_t keyboard = malloc(sizeof(struct keyboard));
	keyboard_init(keyboard);
	return keyboard;
}

void keyboard_delete(keyboard_t keyboard) {
	free(keyboard);
}

//MARK: - Key Management

/* Key codes in z80e are group << 4 | bit. That is, 0x14 is bit 4 of group 1. */

void keyboard_press(keyboard_t keyboard, uint8_t keycode) {
	uint8_t group = keycode >> 4;
	uint8_t mask = 1 << (keycode & 0xF);
	keyboard->groups[group] &= ~mask;
}

void keyboard_release(keyboard_t keyboard, uint8_t keycode) {
	uint8_t group = keycode >> 4;
	uint8_t mask = 1 << (keycode & 0xF);
	keyboard->groups[group] |= mask;
}

//MARK: - Device Management

uint8_t keyboard_read(const keyboard_t keyboard) {
	uint8_t mask = keyboard->group_mask;

	uint8_t value = 0;
	for (int i = 7; i >= 0; i--) {
		if (mask & 0x80)
			value |= ~keyboard->groups[i];
		mask <<= 1;
	}
	return ~value;
}

void keyboard_write(const keyboard_t keyboard, uint8_t value) {
	if (value == 0xFF) {
		keyboard->group_mask = 0;
		return;
	}
	keyboard->group_mask |= ~value;
}

struct z80_device keyboard_device(const keyboard_t keyboard) {
	return (struct z80_device){ keyboard, (read_t)keyboard_read, (write_t)keyboard_write };
}
