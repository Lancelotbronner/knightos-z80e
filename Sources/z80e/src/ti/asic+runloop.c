#include <z80e/ti/asic.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#ifndef NOLINK
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>
#include <poll.h>
#endif
#include <errno.h>
/* Why the heck does "get the current time" have to be so god-dammed platform specific */
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#ifdef __APPLE__
#include <mach/mach_time.h>
#define ORWL_NANO (+1.0E-9)
#define ORWL_GIGA UINT64_C(1000000000)
static double orwl_timebase = 0.0;
static uint64_t orwl_timestart = 0;
#endif
#include <z80e/devices/link.h>

long long get_time_nsec() {
#ifdef EMSCRIPTEN
	return emscripten_get_now() * 1000000;
#else
#ifdef __APPLE__
	if (!orwl_timestart) {
		mach_timebase_info_data_t tb = { 0 };
		mach_timebase_info(&tb);
		orwl_timebase = tb.numer;
		orwl_timebase /= tb.denom;
		orwl_timestart = mach_absolute_time();
	}
	struct timespec t;
	double diff = (mach_absolute_time() - orwl_timestart) * orwl_timebase;
	t.tv_sec = diff * ORWL_NANO;
	t.tv_nsec = diff - (t.tv_sec * ORWL_GIGA);
	return t.tv_nsec;
#else
	struct timespec sp;
	clock_gettime(CLOCK_MONOTONIC, &sp);

	return sp.tv_sec * 1000000000 + sp.tv_nsec;
#endif
#endif
}

void runloop_init(asic_t asic) {
	asic->runloop.last_end = get_time_nsec();

	for (int i = 0; i < asic->timers.capacity; i++) {
		z80_timer_t timer = &asic->timers.head[i];
		if (timer->callback)
			timer->cycles_until_tick = asic->clock_rate / timer->frequency;
	}

	asic->runloop.ticks = calloc(sizeof(timer_tick_t), 40);
	asic->runloop.ticks_capacity = 40;
}

int runloop_compare(const void *first, const void *second) {
	const timer_tick_t *a = first;
	const timer_tick_t *b = second;

	return a->after_cycle - b->after_cycle;
}

void link_socket_update(asic_t asic, int c) {
	if (c != EOF) {
		char _c = ' ';
		if (isprint((char)c)) {
			_c = (char)c;
		}
		// TODO: Log the TX better
		printf("Asked to send %02X (%c)\n", c, _c);
	}

#ifndef NOLINK
	if (!asic->socket.listenfd.fd) {
		return;
	}

	struct sockaddr_in clientaddr;
	unsigned size = sizeof(clientaddr);
	poll(&asic->socket.listenfd, 1, 0);
	int fd = -1;
	if (asic->socket.accept++ == 4096) { // stupid hack
		fd = accept(asic->socket.listenfd.fd, (struct sockaddr *)&clientaddr, &size);
		asic->socket.accept = 0;
	}

	int i;
	int length = sizeof(asic->socket.clients) / sizeof(*asic->socket.clients);
	for (i = 0; i < length; ++i) {
		struct pollfd client = asic->socket.clients[i];
		if (asic->socket.clients[i].fd == 0) {
			if (fd != -1) {
				asic->socket.clients[i].fd = fd;
				asic->socket.clients[i].events = POLLIN | POLLHUP;
				printf("Client accepted with fd %d\n", fd);
				fd = -1;
			} else {
				continue;
			}
		}
		if (asic->socket.clients[i].fd != 0) {
			uint8_t val;
			poll(&asic->socket.clients[i], 1, 0);
			if (asic->socket.clients[i].revents & POLLHUP) {
				close(asic->socket.clients[i].fd);
				asic->socket.clients[i].fd = 0;
				continue;
			}
			if (asic->socket.clients[i].revents & POLLIN) {
				if (link_recv_ready(asic)) {
					if (read(asic->socket.clients[i].fd, &val, 1)) {
						link_recv_byte(asic, val);
					}
				}
			}
			if (c != EOF) {
				val = c;
				write(asic->socket.clients[i].fd, &c, 1);
			}
		}
	}
#endif
}

void asic_tick_cycles(asic_t asic, int cycles) {
	int total_cycles = 0;
	int cycles_until_next_tick = cycles;
	int current_tick = 0;
	int i;

	for (i = 0; i < asic->timers.capacity; i++) {
		z80_timer_t timer = &asic->timers.head[i];

		if (!timer->callback)
			continue;

		int tot_cycles = cycles;
		if (timer->cycles_until_tick >= tot_cycles) {
			timer->cycles_until_tick -= tot_cycles;
			continue;
		}

		do {
			asic->runloop.ticks[current_tick].index = i;
			asic->runloop.ticks[current_tick].after_cycle = timer->cycles_until_tick + (cycles - tot_cycles);
			tot_cycles -= timer->cycles_until_tick;
			timer->cycles_until_tick = asic->clock_rate / timer->frequency;
			current_tick++;

			if (current_tick != asic->runloop.ticks_capacity)
				continue;

			asic->runloop.ticks_capacity += 10;
			asic->runloop.ticks = realloc(asic->runloop.ticks, sizeof(timer_tick_t) * asic->runloop.ticks_capacity);

		} while (timer->cycles_until_tick <= tot_cycles);
	}

	qsort(asic->runloop.ticks, current_tick, sizeof(timer_tick_t), runloop_compare);

	if (current_tick > 0)
		cycles_until_next_tick = asic->runloop.ticks[0].after_cycle;

	int tick_i = 0;
	while (cycles > 0) {
		int ran = cycles_until_next_tick - cpu_execute(&asic->cpu, cycles_until_next_tick);
		int c = link_read_tx_buffer(asic);
		link_socket_update(asic, c);

		total_cycles += ran;
		cycles -= ran;

		if (total_cycles < asic->runloop.ticks[tick_i].after_cycle)
			continue;

		if (++tick_i > current_tick) {
			cycles_until_next_tick = cycles;
			continue;
		}

		int index = asic->runloop.ticks[tick_i - 1].index;
		z80_timer_t timer = &asic->timers.head[index];
		timer->callback(asic, timer->data);
		cycles_until_next_tick = asic->runloop.ticks[tick_i].after_cycle - total_cycles;
	}

	asic->runloop.spare_cycles = cycles;
}

void asic_tick(asic_t asic) {
	long long now = get_time_nsec();
	long long ticks_between = now - asic->runloop.last_end;

	float seconds = (float)ticks_between / (float)1000000000;
	int cycles = seconds * (float)asic->clock_rate;

	if (cycles == 0)
		return;

	asic_tick_cycles(asic, cycles);
	asic->runloop.last_end = now;
}
