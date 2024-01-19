#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#define RESET "\x1b[0m"

static struct {
  const char *message;
  const char *fg;
  const char *bg;
} log_level_info[] = {
    {.message = "ERROR", .fg = "\x1b[0;37m", .bg = "\x1b[41m"}, // error
    {.message = "!", .fg = "\x1b[1;33m", .bg = ""},             // warning
    {.message = "*", .fg = "\x1b[1;34m", .bg = ""},             // info
    {.message = "+", .fg = "\x1b[1;32m", .bg = ""},             // success
    {.message = "DEBUG", .fg = "\x1b[1;31m", .bg = ""},         // debug
};

static enum LogLevel current_log_level = Success;

void set_log_level(enum LogLevel level) { current_log_level = level; }

static void do_log(enum LogLevel level, const char *fmt, va_list ap) {

  if (level > current_log_level)
    return;

  // allow output to be redirected without a mess
  if (isatty(fileno(stdout))) {

    printf("[%s%s%s%s] ", log_level_info[level].fg, log_level_info[level].bg,
           log_level_info[level].message, RESET);
  }

  vprintf(fmt, ap);
}

void log_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  do_log(Error, fmt, args);
}

void log_warning(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  do_log(Warning, fmt, args);
}

void log_info(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  do_log(Info, fmt, args);
}

void log_success(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  do_log(Success, fmt, args);
}

void log_debug(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  do_log(Debug, fmt, args);
}
