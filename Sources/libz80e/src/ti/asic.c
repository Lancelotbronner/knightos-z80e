#include <z80e/ti/asic.h>
#include <z80e/log.h>
#include <z80e/cpu/z80.h>
#include <z80e/hardware/mmu.h>
#include <z80e/hardware/t6a04.h>
#include <z80e/hardware/timer.h>
#include <z80e/devices/crystal.h>
#include <z80e/devices/flash.h>
#include <z80e/devices/interrupts.h>
#include <z80e/devices/keyboard.h>
#include <z80e/devices/link.h>
#include <z80e/devices/mapping.h>
#include <z80e/devices/speed.h>
#include <z80e/devices/status.h>
#include <z80e/devices/t6a04.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//MARK: - Lifecycle Management

static void plug_devices(asic_t asic);
static void asic_mirror_ports(asic_t asic);

void asic_init(asic_t asic, ti_device_type type) {
	// Ensure everything is always zero-initialized.
	*asic = (struct asic){};

	// Configure device
	asic->device = type;
	asic->battery.state = BATTERIES_GOOD;

	// Configure the Memory Management Unit
	mmu_init(&asic->mmu, type);

	// Configure the CPU
	asic->cpu.memory = (void*)&asic->mmu;
	asic->cpu.memory_read = (memory16_read8_t)mmu_read;
	asic->cpu.memory_write = (memory16_write8_t)mmu_write;
	asic->clock_rate = 6000000;

	// Configure timers
	asic->timers.capacity = 20;
	asic->timers.head = calloc(20, sizeof(z80_timer_t));

	// Configure the runloop
	runloop_init(asic);

	// Configure interrupts
	asic->interrupts.timer1 = -1;
	asic->interrupts.timer2 = -1;
	asic_power_release(asic);

	// Configure debugging
	asic->stopped = false;

	// Configure devices
	plug_devices(asic);
	asic_mirror_ports(asic);
}

void asic_deinit(asic_t asic) {
	mmu_deinit(&asic->mmu);
}

//MARK: - Port Management

void asic_install(asic_t asic, const device_t device, unsigned char port) {
	asic->cpu.devices[port] = *device;
}

device_t asic_device(asic_t asic, unsigned char port) {
	return &asic->cpu.devices[port];
}

typedef struct {
	asic_t asic;
	uint8_t port;
} unimplemented_port_t;

static uint8_t __unimplemented_read(device_t device) {
	unimplemented_port_t *d = device->data;
	z80e_info("asic", "Warning: attempted to read from unimplemented port 0x%02x from 0x%04X.", d->port, d->asic->cpu.registers.PC);
	return 0x00;
}

static void __unimplemented_write(device_t device, uint8_t value) {
	unimplemented_port_t *d = device->data;
	z80e_info("asic", "Warning: attempted to write 0x%02x to unimplemented port 0x%02x from 0x%04X.", value, d->port, d->asic->cpu.registers.PC);
}

static void plug_devices(asic_t asic) {

	if (asic->device != TI73 && asic->device != TI83p) {
		port_speed(asic_device(asic, 0x20), asic);
		
		// Initialize 3 crystal timers
		
		port_crystal_frequency(asic_device(asic, 0x30));
		port_crystal_loop(asic_device(asic, 0x31));
		port_crystal_count(asic_device(asic, 0x32));

		port_crystal_frequency(asic_device(asic, 0x33));
		port_crystal_loop(asic_device(asic, 0x34));
		port_crystal_count(asic_device(asic, 0x35));

		port_crystal_frequency(asic_device(asic, 0x36));
		port_crystal_loop(asic_device(asic, 0x37));
		port_crystal_count(asic_device(asic, 0x38));
	}

	// Initialize the keyboard
	keyboard_init(&asic->keyboard);
	port_keyboard(asic_device(asic, 0x01), &asic->keyboard);

	// Initialize the status port
	port_status(asic_device(asic, 0x02), asic);

	// Initialize interrupts
	port_interrupt_mask(asic_device(asic, 0x03), asic);

	// Initialize the LCD display
	lcd_t6a04_init(&asic->lcd);
	port_t6a04_status(asic_device(asic, 0x10), &asic->lcd);
	port_t6a04_data(asic_device(asic, 0x11), &asic->lcd);

	// Initialize link ports
	init_link_ports(asic);

	// Initialize memory mapping ports
	mapping_init(&asic->mapping, asic);
	port_mapping_status(asic_device(asic, 0x04), &asic->mapping);
	if (asic->device != TI83p)
		port_mapping_paging(asic_device(asic, 0x05), &asic->mapping);
	port_mapping_bankA(asic_device(asic, 0x06), &asic->mapping);
	port_mapping_bankB(asic_device(asic, 0x07), &asic->mapping);
	mapping_reload(&asic->mapping);

	// Initialize flash ports
	port_flash_control(asic_device(asic, 0x14), asic);
	port_flash_size(asic_device(asic, 0x21));
}

static void asic_mirror_ports(asic_t asic) {
	int i;
	switch (asic->device) {
	case TI83p:
		for (i = 0x08; i < 0x10; i++) {
			port_mirror(asic_device(asic, i), asic_device(asic, i & 0x07));
			port_null(asic_device(asic, i), false, true);
		}
		
		port_mirror(asic_device(asic, 0x12), asic_device(asic, 0x10));
		port_mirror(asic_device(asic, 0x13), asic_device(asic, 0x11));

		port_mirror(asic_device(asic, 0x15), asic_device(asic, 0x05));
		port_null(asic_device(asic, 0x15), false, true);

		for (i = 0x17; i < 0x100; i++) {
			port_mirror(asic_device(asic, i), asic_device(asic, i & 0x07));
			port_null(asic_device(asic, i), false, true);
		}
		break;
	default:
		for (i = 0x60; i < 0x80; i++)
			port_mirror(asic_device(asic, i), asic_device(asic, i - 0x20));
		break;
	}
}

//MARK: - Timer Management

int asic_add_timer(asic_t asic, int flags, double frequency, timer_callback_t tick, void *data) {
	z80_timer_t timer = 0;
	int i;
	for (i = 0; i < asic->timers.capacity; i++) {
		if (!asic->timers.head[i].callback) {
			timer = &asic->timers.head[i];
			break;
		}

		if (i == asic->timers.capacity - 1) {
			asic->timers.capacity += 10;
			asic->timers.head = realloc(asic->timers.head, sizeof(z80_timer_t) * asic->timers.capacity);
			z80_timer_t ne = &asic->timers.head[asic->timers.capacity - 10];
			memset(ne, 0, sizeof(z80_timer_t) * 10);
		}
	}

	timer->cycles_until_tick = asic->clock_rate / frequency;
	timer->frequency = frequency;
	timer->callback = tick;
	timer->data = data;
	return i;
}

void asic_remove_timer(asic_t asic, int index) {
	asic->timers.head[index].callback = nullptr;
}

int asic_set_clock_rate(asic_t asic, int new_rate) {
	int old_rate = asic->clock_rate;

	int i;
	for (i = 0; i < asic->timers.capacity; i++) {
		z80_timer_t timer = &asic->timers.head[i];
		if (timer->callback)
			timer->cycles_until_tick = new_rate / (timer->cycles_until_tick * timer->frequency);
	}

	asic->clock_rate = new_rate;
	return old_rate;
}
