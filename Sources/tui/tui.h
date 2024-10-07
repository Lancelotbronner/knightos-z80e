#pragma once

#include <z80e/ti/asic.h>
#include <z80e/debugging/debugger.h>

void tui_init(debugger_t debugger);
void tui_tick(debugger_t debugger);
