#pragma once

#include <z80e/types.h>

typedef struct keyboard_device *keyboard_device_t;

struct keyboard_device {
	unsigned char group_mask;
	unsigned char groups[8];
};

/// Configures a device to be a Ti keyboard.
/// - Parameters:
///   - device: The device to configure.
void device_keyboard(device_t device);

//MARK: - Keyboard Management

void keyboard_release(keyboard_device_t keyboard, unsigned char keycode);
void keyboard_press(keyboard_device_t keyboard, unsigned char keycode);
