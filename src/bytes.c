#include "assertions.h"
#include "kpwn.h"
#include <gc/gc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Bytes *b_new(size_t n) {
  Bytes *self = GC_MALLOC_ATOMIC(sizeof(Bytes) + n);

  VERIFY(self != NULL);

  self->_len = n;
  return self;
}

Bytes *b_new_v(const uint8_t *bytes, size_t n) {

  Bytes *self = b_new(n);

  memcpy(self->_data, bytes, n);
  return self;
}

Bytes *b_slice(const Bytes *original, size_t start, size_t end) {

  const uint8_t *start_ptr = b_s(original) + posmod(start, b_len(original));

  size_t len = posmod(end, b_len(original)) - posmod(start, b_len(original));

  Bytes *self = b_new_v(start_ptr, len);
  return self;
}
