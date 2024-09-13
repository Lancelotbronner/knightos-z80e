#pragma once

typedef enum {
	L_ERROR = 0,
	L_WARN = 1,
	L_INFO = 2,
	L_DEBUG = 3,
} loglevel_t;

typedef void (*log_callback_t)(void *data, loglevel_t level, const char *domain, const char *message);

void z80_log_callback(log_callback_t callback, void *data);
void z80_log_filter(loglevel_t level);

void z80_log(loglevel_t level, const char *domain, const char *format, ...);
void z80_debug(const char *domain, const char *format, ...);
void z80_info(const char *domain, const char *format, ...);
void z80_warning(const char *domain, const char *format, ...);
void z80_error(const char *domain, const char *format, ...);
