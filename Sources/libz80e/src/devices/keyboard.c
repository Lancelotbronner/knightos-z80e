#include <z80e/peripherals/keyboard.h>

#include <z80e/peripheral.h>

//MARK: - Keyboard Management

/* Key codes in z80e are group << 4 | bit. That is, 0x14 is bit 4 of group 1. */

void keyboard_init(keyboard_device_t keyboard) {
	*keyboard = (struct keyboard_device){
		.group_mask = 0,
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

static unsigned char __keyboard_read(const peripheral_t peripheral) {
	keyboard_device_t keyboard = peripheral->data;
	unsigned char mask = keyboard->group_mask;

	unsigned char value = 0;
	for (int i = 7; i >= 0; i--) {
		if (mask & 0x80)
			value |= ~keyboard->groups[i];
		mask <<= 1;
	}
	return ~value;
}

static void __keyboard_write(const peripheral_t peripheral, unsigned char value) {
	keyboard_device_t keyboard = peripheral->data;
	if (value == 0xFF) {
		keyboard->group_mask = 0;
		return;
	}
	keyboard->group_mask |= ~value;
}

void port_keyboard(peripheral_t peripheral, const keyboard_device_t keyboard) {
	peripheral->data = keyboard;
	peripheral->read = __keyboard_read;
	peripheral->write = __keyboard_write;
}
