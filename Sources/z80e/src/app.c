//
//  app.c
//  z80e
//
//  Created by Christophe Bronner on 2024-11-08.
//

#include "app.h"

#include <z80e/log.h>
#include <z80e/tui.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//MARK: - Application Context

static struct asic Asic;
static struct debugger Debugger;

struct z80e {
	ti_device_type device;
	char *rom_file;
	int cycles;

	bool print_state : 1;
	bool no_rom_check : 1;
	bool debugging : 1;
};

static struct z80e z80e = {
	.device = TI83p,
	.rom_file = nullptr,
	.cycles = -1,
	.print_state = false,
	.no_rom_check = false,
	.debugging = false,
};

asic_t z80e_asic() {
	return &Asic;
}

debugger_t z80e_debugger() {
	return &Debugger;
}

bool z80e_init() {
	signal(SIGINT, z80e_sigint);
	disassembler_init();
	asic_init(&Asic, z80e.device);

	if (!z80e_load_rom())
		goto failure;

	if (!z80e.rom_file) {
		fprintf(stderr, "No ROM file specified, starting debugger.");
		z80e.debugging = true;
	}

	if (z80e.debugging) {
		debugger_init(&Debugger, &Asic);
		Debugger.state = DEBUGGER_ENABLED;
	}

	return true;
failure:
	asic_deinit(&Asic);
	return false;
}

void z80e_start() {
	if (Debugger.state == DEBUGGER_ENABLED) {
		tui_init(&Debugger);
		tui_tick(&Debugger);
		return;
	}

	if (z80e.cycles >= 0) {
		asic_tick_cycles(&Asic, z80e.cycles);
		return;
	}

	while (true) {
		asic_tick(&Asic);
		if (Asic.stopped)
			break;
		nanosleep((struct timespec[]){{0, (1.f / 60.f) * 1000000000}}, NULL);
	}

}

void z80e_deinit() {
	if (z80e.print_state)
		print_state(&Asic.cpu);

	asic_deinit(&Asic);
	debugger_deinit(&Debugger);
}

//MARK: - Signal Handling

static void z80e_sigint(int sig) {
	signal(SIGINT, z80e_sigint);

	z80e_error("sigint", "Caught interrupt, stopping emulation");
	Asic.stopped = true;

	if (!z80e.debugging || Debugger.state == DEBUGGER_ENABLED) {
#ifdef CURSES
		endwin();
#endif
		exit(EXIT_SUCCESS);
	}

	fflush(stdout);
}

//MARK: - Configuration Management

void z80e_check_rom(bool enable) {
	z80e.no_rom_check = !enable;
}

void z80e_cycles(int cycles) {
	z80e.cycles = cycles;
}

void z80e_device(ti_device_type device) {
	z80e.device = device;
}

void z80e_debugging(bool enabled) {
	z80e.debugging = true;
}

void z80e_print_state(bool enable) {
	z80e.print_state = enable;
}

void z80e_rom(char *filepath) {
	z80e.rom_file = filepath;
}

//MARK: - Help

void z80e_help(bool should_exit) {
	printf("z80e - Emulate z80 calculators\n"
		   "Usage: z80e [flags] [rom]\n\n"
		   "Flags:\n"
		   "\t-d <device>: Selects a device to emulate. Available devices:\n"
		   "\t\tTI73, TI83p, TI83pSE, TI84p, TI84pSE, TI84pCSE\n"
		   "\t-c <cycles>: Emulate this number of cycles, then exit. If omitted, the machine will be emulated indefinitely.\n"
		   "\t--print-state: Prints the state of the machine on exit.\n"
		   "\t--no-rom-check: Skips the check that ensure the provided ROM file is the correct size.\n"
		   "\t--debug: Enable the debugger, which is enabled by interrupting the emulator.\n");
	if (should_exit)
		exit(EXIT_SUCCESS);
}

//MARK: - Argument Management

static int argi;
static int argc;
static char **argv;

static bool z80e_parse_param(char *arg) {
	if (!z80e.rom_file) {
		z80e_rom(arg);
		return true;
	}
	return false;
}

static bool z80e_parse_device(char *arg) {
	if (*arg != 'T' && *arg != 't')
		return false;
	if (arg++, *arg != 'I' && *arg != 'i')
		return false;
	arg++;
	if (strcasecmp(arg, "73") == 0)
		z80e_device(TI73);
	else if (strcasecmp(arg, "83p") == 0)
		z80e_device(TI83p);
	else if (strcasecmp(arg, "83pSE") == 0)
		z80e_device(TI83pSE);
	else if (strcasecmp(arg, "84p") == 0)
		z80e_device(TI84p);
	else if (strcasecmp(arg, "84pSE") == 0)
		z80e_device(TI84pSE);
	else if (strcasecmp(arg, "84pCSE") == 0)
		z80e_device(TI84pCSE);

	fprintf(stderr, "'%s' is not a valid device.\n", arg);
	fprintf(stderr, "Valid devices are: TI73, TI83p, TI83pSE, TI84p, TI84pSE, TI84pCSE.\n");
	return false;
}

static bool z80e_parse_log(char *arg) {
	if (strcasecmp(arg, "err") == 0)
		z80e_log_filter(L_ERROR);
	else if (strcasecmp(arg, "error") == 0)
		z80e_log_filter(L_ERROR);
	else if (strcasecmp(arg, "debug") == 0)
		z80e_log_filter(L_DEBUG);
	else if (strcasecmp(arg, "info") == 0)
		z80e_log_filter(L_INFO);
	else if (strcasecmp(arg, "warn") == 0)
		z80e_log_filter(L_WARN);
	else if (strcasecmp(arg, "warning") == 0)
		z80e_log_filter(L_WARN);
	fprintf(stderr, "%s is not a valid logging level.\n", arg);
	fprintf(stderr, "Valid levels are: debug, info, warning, error.\n");
}

static bool z80e_parse_long(char *arg) {
	if (strcasecmp(arg, "device") == 0)
		z80e_parse_device(argv[++argi]);
	else if (strcasecmp(arg, "print-state") == 0)
		z80e_print_state(true);
	else if (strcasecmp(arg, "no-rom-check") == 0)
		z80e_check_rom(false);
	else if (strcasecmp(arg, "log") == 0)
		z80e_parse_log(argv[++argi]);
	else if (strcasecmp(arg, "debug") == 0)
		z80e_debugging(true);
	else if (strcasecmp(arg, "help") == 0)
		z80e_help(true);
	else
		return false;
}

static bool z80e_parse_flag(char arg) {
	switch (arg) {
	case 'd':
		z80e_parse_device(argv[++argi]);
		break;
	case 'c':
		z80e_cycles(atoi(argv[++argi]));
		break;
	default:
		return false;
	}
}

static bool z80e_parse_arg(char *arg) {
	if (*arg != '-')
		return z80e_parse_param(arg);

	arg++;
	if (*arg == '-')
		return z80e_parse_long(arg);
	else
		while (*arg)
			if (!z80e_parse_flag(*arg++))
				return false;

	return true;
}

void z80e_args(int _argc, char *_argv[]) {
	argc = _argc;
	argv = _argv;
	for (argi = 1; argi < argc; argi++)
		if (!z80e_parse_arg(argv[argi])) {
			printf("Incorrect usage. See z80e --help.\n");
			return EXIT_FAILURE;
		}
}

//MARK: - ROM Management

static bool z80e_load_rom() {
	if (!z80e.rom_file)
		return true;

	FILE *file = fopen(z80e.rom_file, "r");
	if (!file) {
		printf("Error opening '%s'.\n", z80e.rom_file);
		return false;
	}

	long length;
	fseek(file, 0L, SEEK_END);
	length = ftell(file);
	fseek(file, 0L, SEEK_SET);

	if (!z80e.no_rom_check && length != Asic.mmu.settings.flash_pages * 0x4000) {
		fprintf(stderr, "Expected ROM file to match the required size of %d bytes but it is %ld bytes. (use --no-rom-check to override).\n",
			   Asic.mmu.settings.flash_pages * 0x4000, length);
		fclose(file);
		return false;
	}

	length = fread(Asic.mmu.flash, 0x4000, Asic.mmu.settings.flash_pages, file);
	fclose(file);
	return true;
}

//MARK: - LCD Management

static bool DirtyLCD = true;

static void on_lcd_update(void *data, lcd_t6a04_t lcd) {
	DirtyLCD = true;
}

static void on_lcd_refresh(asic_t asic, void *data) {
	if (!DirtyLCD)
		return;
	DirtyLCD = false;

	hook_lcd_callback_t callback = data;
	callback(&asic, &asic->lcd);
}

void z80e_dirty_lcd() {
	DirtyLCD = true;
}

bool z80e_hook_lcd(float refresh_rate, hook_lcd_callback_t callback) {
	hook_lcd_emplace(&Asic.lcd.hook.update, nullptr, on_lcd_update);
	asic_add_timer(&Asic, 0, refresh_rate, on_lcd_refresh, callback);
}
