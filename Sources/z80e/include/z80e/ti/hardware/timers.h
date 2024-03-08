#pragma once

#include <stdint.h>

#include <z80e/ti/types.h>

struct crystal_timer {
    asic_t *asic;
    uint8_t frequency;
    uint8_t loop;
    uint8_t counter;
};

void init_crystal_timers(asic_t *asic);
void free_crystal_timers(asic_t *asic);
