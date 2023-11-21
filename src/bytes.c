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

  size_t len =
      posmod(end, b_len(original)) - posmod(start, b_len(original)) + 1;

  Bytes *self = b_new_v(start_ptr, len);
  return self;
}

int b_cmp(const Bytes *b1, const Bytes *b2) {
  if (b_len(b1) != b_len(b2))
    return 1;
  return memcmp(b_s(b1), b_s(b2), b_len(b1));
}
