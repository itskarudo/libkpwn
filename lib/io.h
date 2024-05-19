#pragma once

#include "bytes.h"
#include <fcntl.h>

typedef struct {
  int _fd;
} IO;

#ifdef __cplusplus
extern "C" {
#endif

IO *io_new(const char *path, int flags);
void io_close(IO *);

Bytes *io_read(IO *, size_t n);
Bytes *io_readuntil(IO *, Bytes *delim);
Bytes *io_readline(IO *);

void io_write(IO *, Bytes *);

int io_ioctl(IO *, unsigned long request, ...);

#ifdef __cplusplus
}
#endif
