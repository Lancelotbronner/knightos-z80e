#include <z80e/ti/asic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <z80e/log/log.h>
#include <z80e/cpu/z80.h>
#include <z80e/ti/memory.h>
#include <z80e/hardware/t6a04.h>
#include <z80e/devices/flash.h>
#include <z80e/devices/interrupts.h>
#include <z80e/devices/keyboard.h>
#include <z80e/devices/link.h>
#include <z80e/devices/mapping.h>
#include <z80e/devices/speed.h>
#include <z80e/devices/status.h>
#include <z80e/devices/crystal.h>

//MARK: - Lifecycle Management

static void plug_devices(asic_t asic);
static void asic_mirror_ports(asic_t asic);

void asic_init(asic_t asic, ti_device_type type) {
	// Ensure everything is always zero-initialized.
	*asic = (struct asic){};

	// Configure device
	asic->device = type;
	asic->battery = BATTERIES_GOOD;

	// Configure the Memory Management Unit
	ti_mmu_init(&asic->mmu, type);

	// Configure the CPU
	cpu_init(&asic->cpu);
	asic->cpu.memory = (void*)&asic->mmu;
	asic->cpu.read_byte = ti_read_byte;
	asic->cpu.write_byte = ti_write_byte;
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

	// Configure debuggers
	asic->stopped = 0;
	asic->debugger = 0;
	asic->hook = create_hook_set(asic);

	// Configure devices
	plug_devices(asic);
	asic_mirror_ports(asic);
}

void asic_deinit(asic_t asic) {
	ti_mmu_deinit(&asic->mmu);
}

//MARK: - Device Management

void asic_install(asic_t asic, const device_t device, unsigned char port) {
	asic->cpu.devices[port] = *device;
}

device_t asic_device(asic_t asic, unsigned char port) {
	return &asic->cpu.devices[port];
}

typedef struct {
	asic_t asic;
	uint8_t port;
} unimplemented_device_t;

static uint8_t __unimplemented_read(device_t device) {
	unimplemented_device_t *d = device->data;
	z80_info("asic", "Warning: attempted to read from unimplemented port 0x%02x from 0x%04X.", d->port, d->asic->cpu.registers.PC);
	return 0x00;
}

static void __unimplemented_write(device_t device, uint8_t value) {
	unimplemented_device_t *d = device->data;
	z80_info("asic", "Warning: attempted to write 0x%02x to unimplemented port 0x%02x from 0x%04X.", value, d->port, d->asic->cpu.registers.PC);
}

static void plug_devices(asic_t asic) {

	if (asic->device != TI73 && asic->device != TI83p) {
		device_speed(asic_device(asic, 0x20), asic);
		
		// Initialize 3 crystal timers
		
		device_crystal_frequency(asic_device(asic, 0x30));
		device_crystal_loop(asic_device(asic, 0x31));
		device_crystal_count(asic_device(asic, 0x32));

		device_crystal_frequency(asic_device(asic, 0x33));
		device_crystal_loop(asic_device(asic, 0x34));
		device_crystal_count(asic_device(asic, 0x35));

		device_crystal_frequency(asic_device(asic, 0x36));
		device_crystal_loop(asic_device(asic, 0x37));
		device_crystal_count(asic_device(asic, 0x38));
	}

	// Initialize the keyboard
	keyboard_init(&asic->keyboard);
	device_keyboard(asic_device(asic, 0x01), &asic->keyboard);

	// Initialize the status port
	device_status(asic_device(asic, 0x02), asic);

	// Initialize interrupts
	device_interrupt_mask(asic_device(asic, 0x03), asic);

	// Initialize the LCD display
	setup_lcd_display(asic, asic->hook);

	// Initialize link ports
	init_link_ports(asic);

	// Initialize memory mapping ports
	mapping_init(&asic->mapping, asic);
	device_mapping_status(asic_device(asic, 0x04), &asic->mapping);
	if (asic->device != TI83p)
		device_mapping_paging(asic_device(asic, 0x05), &asic->mapping);
	device_mapping_bankA(asic_device(asic, 0x06), &asic->mapping);
	device_mapping_bankB(asic_device(asic, 0x07), &asic->mapping);
	mapping_reload(&asic->mapping);

	// Initialize flash ports
	device_flash_control(asic_device(asic, 0x14), asic);
	device_flash_size(asic_device(asic, 0x21));
}

static void asic_mirror_ports(asic_t asic) {
	int i;
	switch (asic->device) {
	case TI83p:
		for (i = 0x08; i < 0x10; i++) {
			device_mirror(asic_device(asic, i), asic_device(asic, i & 0x07));
			device_null(asic_device(asic, i), false, true);
		}
		
		device_mirror(asic_device(asic, 0x12), asic_device(asic, 0x10));
		device_mirror(asic_device(asic, 0x13), asic_device(asic, 0x11));

		device_mirror(asic_device(asic, 0x15), asic_device(asic, 0x05));
		device_null(asic_device(asic, 0x15), false, true);

		for (i = 0x17; i < 0x100; i++) {
			device_mirror(asic_device(asic, i), asic_device(asic, i & 0x07));
			device_null(asic_device(asic, i), false, true);
		}
		break;
	default:
		for (i = 0x60; i < 0x80; i++)
			device_mirror(asic_device(asic, i), asic_device(asic, i - 0x20));
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
