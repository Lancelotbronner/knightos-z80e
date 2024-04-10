#include <z80e/devices/keyboard.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <z80e/ti/asic.h>
#include <z80e/cpu/z80.h>

//MARK: - Keyboard Management

/* Key codes in z80e are group << 4 | bit. That is, 0x14 is bit 4 of group 1. */

void keyboard_press(keyboard_device_t keyboard, uint8_t keycode) {
	uint8_t group = keycode >> 4;
	uint8_t mask = 1 << (keycode & 0xF);
	keyboard->groups[group] &= ~mask;
}

void keyboard_release(keyboard_device_t keyboard, uint8_t keycode) {
	uint8_t group = keycode >> 4;
	uint8_t mask = 1 << (keycode & 0xF);
	keyboard->groups[group] |= mask;
}

//MARK: - Device Management

static uint8_t __keyboard_read(const device_t device) {
	keyboard_device_t keyboard = (keyboard_device_t)device;
	uint8_t mask = keyboard->group_mask;

	uint8_t value = 0;
	for (int i = 7; i >= 0; i--) {
		if (mask & 0x80)
			value |= ~keyboard->groups[i];
		mask <<= 1;
	}
	return ~value;
}

static void __keyboard_write(const device_t device, uint8_t value) {
	keyboard_device_t keyboard = (keyboard_device_t)device;
	if (value == 0xFF) {
		keyboard->group_mask = 0;
		return;
	}
	keyboard->group_mask |= ~value;
}

void device_keyboard(device_t device) {
	device->read = __keyboard_read;
	device->write = __keyboard_write;

	keyboard_device_t keyboard = (keyboard_device_t)device;
	for (int i = 0; i < 8; i++)
		keyboard->groups[i] = 0xFF;
	keyboard->group_mask = 0;
}
