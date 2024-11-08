//
//  devices/keyboard.h
//  z80e
//
//  Created by Christophe Bronner on 2024-04-10.
//

#pragma once

#include <z80e/types.h>

struct keyboard_device {
	unsigned char mask;
	union {
		unsigned char groups[8];
		unsigned long long keys;
	};
};

void keyboard_init(keyboard_device_t keyboard);

void keyboard_release(keyboard_device_t keyboard, unsigned char keycode);

void keyboard_press(keyboard_device_t keyboard, unsigned char keycode);

//MARK: - Port Configuration

/// Configures a port to be a Ti keyboard.
/// - Parameters:
///   - device: The device to configure.
///   - keyboard: The keyboard to connect to this port.
void port_keyboard(device_t device, const keyboard_device_t keyboard);
