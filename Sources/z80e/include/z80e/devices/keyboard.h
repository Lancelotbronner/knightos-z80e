#pragma once

#include <stdint.h>

#include <z80e/types.h>

struct keyboard {
	uint8_t group_mask;
	uint8_t groups[8];
};

void keyboard_init(keyboard_t keyboard);

keyboard_t keyboard_new();
void keyboard_delete(keyboard_t keyboard);

void keyboard_release(keyboard_t keyboard, uint8_t keycode);
void keyboard_press(keyboard_t keyboard, uint8_t keycode);

//MARK: - Device Management

uint8_t keyboard_read(keyboard_t keyboard);

void keyboard_write(keyboard_t keyboard, uint8_t value);

struct z80_device keyboard_device(const keyboard_t keyboard);
