#include <z80e/log.h>

#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

const char *loglevel_to_string(loglevel_t level) {
	switch (level) {
	case L_DEBUG:
		return "DEBUG";
	case L_INFO:
		return "INFO";
	case L_WARN:
		return "WARN";
	case L_ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}

//TODO: Support os_log?

#define LOG_CAPACITY 1024
static log_callback_t Callback;
static void *Data;
static loglevel_t Level;
static char Message[LOG_CAPACITY];

void z80e_log_callback(log_callback_t callback, void *data) {
	Callback = callback;
	Data = data;
}

void z80e_log_filter(loglevel_t level) {
	Level = level;
}

static void Log(loglevel_t level, const char *domain, const char *format, va_list args) {
	vsnprintf(Message, LOG_CAPACITY, format, args);
	Callback(Data, level, domain, Message);
}

#define ImplementLog(level) \
if (level > Level) return; \
va_list args; \
va_start(args, format); \
Log(level, domain, format, args)

void z80e_log(loglevel_t level, const char *domain, const char *format, ...) {
	ImplementLog(level);
}

void z80e_debug(const char *domain, const char *format, ...) {
	ImplementLog(L_DEBUG);
}

void z80e_info(const char *domain, const char *format, ...) {
	ImplementLog(L_INFO);
}

void z80e_warning(const char *domain, const char *format, ...) {
	ImplementLog(L_WARN);
}

void z80e_error(const char *domain, const char *format, ...) {
	ImplementLog(L_ERROR);
}
