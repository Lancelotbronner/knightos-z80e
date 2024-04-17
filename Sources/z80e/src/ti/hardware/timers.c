/*
 * Implements the 83+SE/84+((C)SE) crystal timers
 */
#include <z80e/ti/asic.h>
#include <z80e/log/log.h>
#include <z80e/ti/memory.h>
#include <z80e/devices/crystal.h>
#include <z80e/ti/hardware/interrupts.h>

#include <stdlib.h>
#include <string.h>

uint8_t read_timer_freq_port(device_t device) {
	struct crystal_timer *timer = device->data;
	return timer->frequency;
}

uint8_t read_timer_loop_port(device_t device) {
	struct crystal_timer *timer = device->data;
	return timer->loop;
}

uint8_t read_timer_counter_port(device_t device) {
	struct crystal_timer *timer = device->data;
	return timer->counter;
}

void write_timer_freq_port(device_t device, uint8_t val) {
	struct crystal_timer *timer = device->data;
	timer->frequency = val;
}

void write_timer_loop_port(device_t device, uint8_t val) {
	struct crystal_timer *timer = device->data;
	timer->loop = val;
}

void write_timer_counter_port(device_t device, uint8_t val) {
	struct crystal_timer *timer = device->data;
	timer->counter = val;
}

void init_crystal_timers(asic_t *asic) {
	struct crystal_timer *timer_1 = calloc(1, sizeof(struct crystal_timer));
	struct crystal_timer *timer_2 = calloc(1, sizeof(struct crystal_timer));
	struct crystal_timer *timer_3 = calloc(1, sizeof(struct crystal_timer));
	timer_1->asic = asic;
	timer_2->asic = asic;
	timer_3->asic = asic;

	struct z80_device timer1_freq_port = { timer_1, read_timer_freq_port, write_timer_freq_port };
	struct z80_device timer1_loop_port = { timer_1, read_timer_loop_port, write_timer_loop_port };
	struct z80_device timer1_count_port = { timer_1, read_timer_counter_port, write_timer_counter_port };

	struct z80_device timer2_freq_port = { timer_2, read_timer_freq_port, write_timer_freq_port };
	struct z80_device timer2_loop_port = { timer_2, read_timer_loop_port, write_timer_loop_port };
	struct z80_device timer2_count_port = { timer_2, read_timer_counter_port, write_timer_counter_port };

	struct z80_device timer3_freq_port = { timer_3, read_timer_freq_port, write_timer_freq_port };
	struct z80_device timer3_loop_port = { timer_3, read_timer_loop_port, write_timer_loop_port };
	struct z80_device timer3_count_port = { timer_3, read_timer_counter_port, write_timer_counter_port };

	asic->cpu.devices[0x30] = timer1_freq_port;
	asic->cpu.devices[0x31] = timer1_loop_port;
	asic->cpu.devices[0x32] = timer1_count_port;

	asic->cpu.devices[0x33] = timer2_freq_port;
	asic->cpu.devices[0x34] = timer2_loop_port;
	asic->cpu.devices[0x35] = timer2_count_port;

	asic->cpu.devices[0x36] = timer3_freq_port;
	asic->cpu.devices[0x37] = timer3_loop_port;
	asic->cpu.devices[0x38] = timer3_count_port;
}

void free_crystal_timers(asic_t *asic) {
	free(asic->cpu.devices[0x30].data);
	free(asic->cpu.devices[0x33].data);
	free(asic->cpu.devices[0x38].data);
}
