#pragma once

#include "bytes.h"
#include <stdio.h>
typedef struct {
  FILE *_file;
} IO;

IO *io_new(const char *path);

Bytes *io_read(IO *, size_t n);
Bytes *io_readuntil(IO *, char delim);
Bytes *io_readline(IO *);

void io_write(IO *, Bytes *);

int io_ioctl(IO *, unsigned long request, ...);
