//
//  app.h
//  z80e
//
//  Created by Christophe Bronner on 2024-11-08.
//

#include <z80e/app.h>

static void z80e_sigint(int sig);

static bool z80e_parse_device(char *arg);
static bool z80e_parse_print_state(char *arg);
static bool z80e_parse_no_rom(char *arg);
static bool z80e_parse_log(char *arg);

static bool z80e_load_rom();
