//
//  log.h
//  z80e
//
//  Created by Christophe Bronner on 2024-09-12.
//

#pragma once

typedef enum {
	L_ERROR = 0,
	L_WARN = 1,
	L_INFO = 2,
	L_DEBUG = 3,
} loglevel_t;

typedef void (*log_callback_t)(void *data, loglevel_t level, const char *domain, const char *message);

void z80e_log_callback(log_callback_t callback, void *data);
void z80e_log_filter(loglevel_t level);

void z80e_log(loglevel_t level, const char *domain, const char *format, ...);
void z80e_debug(const char *domain, const char *format, ...);
void z80e_info(const char *domain, const char *format, ...);
void z80e_warning(const char *domain, const char *format, ...);
void z80e_error(const char *domain, const char *format, ...);
