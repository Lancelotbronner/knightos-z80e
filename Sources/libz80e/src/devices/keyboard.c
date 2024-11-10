#include <z80e/devices/keyboard.h>

#include <z80e/device.h>

//MARK: - Keyboard Management

/* Key codes in z80e are group << 4 | bit. That is, 0x14 is bit 4 of group 1. */

void keyboard_init(keyboard_device_t keyboard) {
	*keyboard = (struct keyboard_device){
		.mask = 0,
		.keys = ~0,
	};
}

void keyboard_press(keyboard_device_t keyboard, unsigned char keycode) {
	unsigned char group = keycode >> 4;
	unsigned char mask = 1 << (keycode & 0xF);
	keyboard->groups[group] &= ~mask;
}

void keyboard_release(keyboard_device_t keyboard, unsigned char keycode) {
	unsigned char group = keycode >> 4;
	unsigned char mask = 1 << (keycode & 0xF);
	keyboard->groups[group] |= mask;
}

//MARK: - Keyboard Port

static unsigned char __keyboard_read(const device_t device) {
	keyboard_device_t keyboard = device->data;
	unsigned char mask = keyboard->mask;

	unsigned char value = 0;
	for (int i = 7; i >= 0; i--) {
		if (mask & 0x80)
			value |= ~keyboard->groups[i];
		mask <<= 1;
	}
	return ~value;
}

static void __keyboard_write(const device_t device, unsigned char value) {
	keyboard_device_t keyboard = device->data;
	if (value == 0xFF) {
		keyboard->mask = 0;
		return;
	}
	keyboard->mask |= ~value;
}

void port_keyboard(device_t device, const keyboard_device_t keyboard) {
	device->data = keyboard;
	device->read = __keyboard_read;
	device->write = __keyboard_write;
}
