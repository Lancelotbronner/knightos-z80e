#pragma once

#include <z80e/types.h>

struct keyboard_device {
	unsigned char mask;
	union {
		unsigned char groups[8];
		unsigned long long keys;
	};
};

//MARK: - Keyboard Management

void keyboard_init(keyboard_device_t keyboard);

void keyboard_release(keyboard_device_t keyboard, unsigned char keycode);
void keyboard_press(keyboard_device_t keyboard, unsigned char keycode);

//MARK: - Device Configuration

/// Configures a device to be a Ti keyboard.
/// - Parameters:
///   - device: The device to configure.
void device_keyboard(device_t device, keyboard_device_t keyboard);
