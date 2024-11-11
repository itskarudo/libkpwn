#include "io.h"
#include "utils.h"
#include <fcntl.h>
#include <gc/gc.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

void io_finalizer(IO *self, void *user_data) {
  (void)user_data;

  close(self->_fd);

  free(self);
}

IO *io_new(const char *path, int flags) {
  IO *self = GC_MALLOC_ATOMIC(sizeof(IO));
  if (self == NULL)
    return NULL;

  self->_fd = open(path, flags);
  if (self->_fd == -1)
    return NULL;

  GC_register_finalizer(self, (GC_finalization_proc)io_finalizer, NULL, NULL,
                        NULL);

  return self;
}

void io_close(IO *self) { close(self->_fd); }

Bytes *io_read(IO *self, size_t n) {

  Bytes *buffer = b_new(n);
  if (buffer == NULL)
    return NULL;

  buffer->_len = read(self->_fd, buffer->_data, n);

  return buffer;
}

Bytes *io_readuntil(IO *self, Bytes *delim) {

  Bytes *buffer = io_read(self, b_len(delim));

  while (b_cmp(b_slice(buffer, -b_len(delim), -1), delim) != 0) {
    Bytes *tmp = io_read(self, 1);
    buffer = flat(buffer, tmp, NULL);
  }

  return buffer;
}

Bytes *io_readline(IO *self) { return io_readuntil(self, b("\n")); }

void io_write(IO *self, Bytes *v) { write(self->_fd, v->_data, v->_len); }

int io_ioctl(IO *self, unsigned long request, ...) {
  va_list args;
  va_start(args, request);
  return ioctl(self->_fd, request, va_arg(args, void *));
}
