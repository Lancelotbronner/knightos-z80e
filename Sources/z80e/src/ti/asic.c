#include <z80e/ti/asic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <z80e/log/log.h>
#include <z80e/cpu/z80.h>
#include <z80e/ti/memory.h>
#include <z80e/ti/hardware/t6a04.h>
#include <z80e/ti/hardware/speed.h>
#include <z80e/ti/hardware/memorymapping.h>
#include <z80e/devices/flash.h>
#include <z80e/devices/keyboard.h>
#include <z80e/devices/status.h>
#include <z80e/ti/hardware/link.h>
#include <z80e/ti/hardware/timers.h>

typedef struct {
	asic_t *asic;
	uint8_t port;
} unimplemented_device_t;

uint8_t read_unimplemented_port(device_t device) {
	unimplemented_device_t *d = device->data;
	log_message(d->asic->log, L_INFO, "asic",
				"Warning: attempted to read from unimplemented port 0x%02x from 0x%04X.", d->port, d->asic->cpu.registers.PC);
	return 0x00;
}

void write_unimplemented_port(device_t device, uint8_t value) {
	unimplemented_device_t *d = device->data;
	log_message(d->asic->log, L_INFO, "asic",
				"Warning: attempted to write 0x%02x to unimplemented port 0x%02x from 0x%04X.", value, d->port, d->asic->cpu.registers.PC);
}

void plug_devices(asic_t *asic) {
	/* Unimplemented devices */
	int i;
	for (i = 0; i < 0x100; i++) {
		unimplemented_device_t *d = malloc(sizeof(unimplemented_device_t));
		d->asic = asic;
		d->port = i;
		struct z80_device device = { d, read_unimplemented_port, write_unimplemented_port };
		asic->cpu.devices[i] = device;
	}

	device_keyboard(&asic->cpu.devices[0x01]);
	device_status(&asic->cpu.devices[0x02], asic);
	asic->cpu.devices[0x03] = init_interrupts(asic, &asic->interrupts);
	setup_lcd_display(asic, asic->hook);

	if (asic->device != TI73 && asic->device != TI83p) {
		asic->cpu.devices[0x20] = init_speed(asic);
		init_crystal_timers(asic);
	}

	init_link_ports(asic);
	init_mapping_ports(asic);

	// Initialize flash ports
	device_flash_control(&asic->cpu.devices[0x14], asic);
	device_flash_size(&asic->cpu.devices[0x21]);
}

void asic_null_write(void *ignored, uint8_t value) {}

void asic_mirror_ports(asic_t *asic) {
	int i;
	switch (asic->device) {
	case TI83p:
		for (i = 0x08; i < 0x10; i++) {
			device_mirror(&asic->cpu.devices[i], &asic->cpu.devices[i & 0x07]);
			device_null(&asic->cpu.devices[i], false, true);
		}
		
		device_mirror(&asic->cpu.devices[0x12], &asic->cpu.devices[0x10]);
		device_mirror(&asic->cpu.devices[0x13], &asic->cpu.devices[0x11]);

		device_mirror(&asic->cpu.devices[0x15], &asic->cpu.devices[0x05]);
		device_null(&asic->cpu.devices[0x15], false, true);

		for (i = 0x17; i < 0x100; i++) {
			device_mirror(&asic->cpu.devices[i], &asic->cpu.devices[i & 0x07]);
			device_null(&asic->cpu.devices[i], false, true);
		}
		break;
	default:
		for (i = 0x60; i < 0x80; i++)
			device_mirror(&asic->cpu.devices[i], &asic->cpu.devices[i - 0x20]);
		break;
	}
}

void free_devices(asic_t *asic) {
	/* Link port unimplemented */
	free_mapping_ports(asic);
}

asic_t *asic_init(ti_device_type type, log_t *log) {
	asic_t* device = calloc(1, sizeof(asic_t));
	device->log = log;
	cpu_init(&device->cpu, log);
	ti_mmu_init(&device->mmu, type, log);
	device->cpu.memory = (void*)&device->mmu;
	device->cpu.read_byte = ti_read_byte;
	device->cpu.write_byte = ti_write_byte;
	device->battery = BATTERIES_GOOD;
	device->device = type;
	device->clock_rate = 6000000;

	device->timers = calloc(1, sizeof(z80_hardware_timers_t));
	device->timers->max_timers = 20;
	device->timers->timers = calloc(20, sizeof(z80_hardware_timer_t));

	device->stopped = 0;
	device->debugger = 0;
	device->runloop = runloop_init(device);
	device->hook = create_hook_set(device);

	device->link = calloc(1, sizeof(z80_link_socket_t));

	plug_devices(device);
	asic_mirror_ports(device);
	return device;
}

void asic_free(asic_t* device) {
	ti_mmu_deinit(&device->mmu);
	free_devices(device);
	free(device);
}

int asic_add_timer(asic_t *asic, int flags, double frequency, timer_tick tick, void *data) {
	z80_hardware_timer_t *timer = 0;
	int i;
	for (i = 0; i < asic->timers->max_timers; i++) {
		if (!(asic->timers->timers[i].flags & TIMER_IN_USE)) {
			timer = &asic->timers->timers[i];
			break;
		}

		if (i == asic->timers->max_timers - 1) {
			asic->timers->max_timers += 10;
			asic->timers->timers = realloc(asic->timers->timers, sizeof(z80_hardware_timer_t) * asic->timers->max_timers);
			z80_hardware_timer_t *ne = &asic->timers->timers[asic->timers->max_timers - 10];
			memset(ne, 0, sizeof(z80_hardware_timer_t) * 10);
		}
	}

	timer->cycles_until_tick = asic->clock_rate / frequency;
	timer->flags = flags | TIMER_IN_USE;
	timer->frequency = frequency;
	timer->on_tick = tick;
	timer->data = data;
	return i;
}

void asic_remove_timer(asic_t *asic, int index) {
	asic->timers->timers[index].flags &= ~TIMER_IN_USE;
}

int asic_set_clock_rate(asic_t *asic, int new_rate) {
	int old_rate = asic->clock_rate;

	int i;
	for (i = 0; i < asic->timers->max_timers; i++) {
		z80_hardware_timer_t *timer = &asic->timers->timers[i];
		if (timer->flags & TIMER_IN_USE) {
			timer->cycles_until_tick =
				new_rate / (timer->cycles_until_tick * timer->frequency);
		}
	}

	asic->clock_rate = new_rate;
	return old_rate;
}
