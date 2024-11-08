#include <z80e/debugging/commands.h>
#include <z80e/debugging/keys.h>
#include <z80e/ti/asic.h>

#include <strings.h>

//MARK: - Press Command

static int command_press(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc != 2) {
		debugger_print(debugger, "%s - Depress the specified key code, in hex or by name.\n", argv[0]);
		return 0;
	}
	uint8_t key;

	int i, found = 0;
	for (i = 0; i < sizeof(key_strings) / sizeof(key_string_t); ++i) {
		if (strcasecmp(key_strings[i].key, argv[1]) == 0) {
			key = key_strings[i].value;
			found = 1;
			break;
		}
	}
	if (!found) {
		key = debugger_evaluate(debugger, argv[1]);
	}

	keyboard_press(&debugger->asic->keyboard, key);
	return 0;
}

const struct debugger_command PressCommand = {
	.name = "press_key",
	.usage = "<key>",
	.summary = "Depresses a key",
	.description = "You can specify `key` by hex or name",
	.callback = command_press,
};

//MARK: - Release Command

static int command_release(debugger_t debugger, void *data, int argc, char **argv) {
	if (argc != 2) {
		debugger_print(debugger, "%s - Release the specified key code, in hex or by name.\n", argv[0]);
		return 0;
	}
	uint8_t key;

	int found = 0;
	for (int i = 0; i < sizeof(key_strings) / sizeof(key_string_t); ++i)
		if (strcasecmp(key_strings[i].key, argv[1]) == 0) {
			key = key_strings[i].value;
			found = 1;
			break;
		}
	if (!found)
		key = debugger_evaluate(debugger, argv[1]);

	keyboard_release(&debugger->asic->keyboard, key);
	return 0;
}

const struct debugger_command ReleaseCommand = {
	.name = "release_key",
	.usage = "<key>",
	.summary = "Releases a key",
	.description = "You can specify `key` by hex or name",
	.callback = command_release,
};

//MARK: - Tap Command

static int command_tap_key(debugger_t debugger, int argc, char **argv) {
	if (argc != 2) {
		debugger_print(debugger, "%s - Depress the specified key code, in hex or by name.\n", argv[0]);
		return 0;
	}
	uint8_t key;

	int i, found = 0;
	for (i = 0; i < sizeof(key_strings) / sizeof(key_string_t); ++i) {
		if (strcasecmp(key_strings[i].key, argv[1]) == 0) {
			key = key_strings[i].value;
			found = 1;
			break;
		}
	}
	if (!found)
		key = debugger_evaluate(debugger, argv[1]);

	keyboard_device_t keyboard = &debugger->asic->keyboard;
	keyboard_press(keyboard, key);
	command_execute(&RunCommand, debugger, 2, "run", "1000");
	keyboard_release(keyboard, key);
	return command_execute(&RunCommand, debugger, 2, "run");
}

const struct debugger_command TapCommand = {
	.name = "tap_key",
	.usage = "<key>",
	.summary = "Press and releases a key",
	.description = "You can specify `key` by hex or name",
	.callback = command_release,
};
