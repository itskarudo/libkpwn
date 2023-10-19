#include "io.h"
#include <fcntl.h>
#include <gc/gc.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

void io_finalizer(IO *self, void *user_data) {
  (void)user_data;

  fclose(self->_file);

  free(self);
}

IO *io_new(const char *path) {
  FILE *file = fopen(path, "rw");

  setvbuf(file, NULL, _IONBF, 0);

  IO *self = GC_MALLOC_ATOMIC(sizeof(IO));
  GC_register_finalizer(self, (GC_finalization_proc)io_finalizer, NULL, NULL,
                        NULL);
  self->_file = file;
  return self;
}

Bytes *io_read(IO *self, size_t n) {

  Bytes *buffer = b_new(n);

  buffer->_len = fread(b_s(buffer), 1, n, self->_file);

  return buffer;
}

Bytes *io_readuntil(IO *self, char delim) {

  char *line = NULL;
  size_t n;

  ssize_t len = getdelim(&line, &n, delim, self->_file);

  Bytes *buffer = b_new_v((uint8_t *)line, len);

  free(line);

  return buffer;
}

Bytes *io_readline(IO *self) { return io_readuntil(self, '\n'); }

void io_write(IO *self, Bytes *v) { fwrite(b_s(v), 1, b_len(v), self->_file); }

int io_ioctl(IO *self, unsigned long request, ...) {
  va_list args;
  va_start(args, request);
  return ioctl(fileno(self->_file), request, va_arg(args, void *));
}
