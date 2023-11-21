#pragma once

#include "bytes.h"
typedef struct {
  int _fd;
} IO;

IO *io_new(const char *path, int flags);

Bytes *io_read(IO *, size_t n);
Bytes *io_readuntil(IO *, Bytes *delim);
Bytes *io_readline(IO *);

void io_write(IO *, Bytes *);

int io_ioctl(IO *, unsigned long request, ...);
