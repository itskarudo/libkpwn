#pragma once

enum LogLevel { Error = 0, Warning = 1, Info = 2, Success = 3, Debug = 4 };

#ifdef __cplusplus
extern "C" {
#endif

void set_log_level(enum LogLevel);

void log_error(const char *fmt, ...);
void log_warning(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_success(const char *fmt, ...);
void log_debug(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
