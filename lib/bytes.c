#include "bytes.h"
#include "utils.h"
#include <gc/gc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Bytes *b_new(size_t n) {
  Bytes *self = GC_MALLOC_ATOMIC(sizeof(Bytes) + n);

  if (self == NULL)
    return NULL;

  self->_len = n;
  return self;
}

Bytes *b_new_v(const uint8_t *bytes, size_t n) {

  Bytes *self = b_new(n);

  memcpy(self->_data, bytes, n);
  return self;
}

Bytes *b_slice(const Bytes *original, size_t start, size_t end) {

  const uint8_t *start_ptr = b_d(original) + posmod(start, b_len(original));

  size_t len =
      posmod(end, b_len(original)) - posmod(start, b_len(original)) + 1;

  Bytes *self = b_new_v(start_ptr, len);
  return self;
}

int b_cmp(const Bytes *b1, const Bytes *b2) {
  if (b_len(b1) != b_len(b2))
    return 1;
  return memcmp(b_d(b1), b_d(b2), b_len(b1));
}

const char *b_s(const Bytes *self) {
  char *s = GC_MALLOC_ATOMIC(b_len(self) * 4 + 4);

  char *ptr = s;
  *ptr++ = 'b';
  *ptr++ = '\'';

  for (size_t i = 0; i < b_len(self); i++) {
    if (b_at(self, i) == '\\') {
      *ptr++ = '\\';
      *ptr++ = '\\';
    } else if (b_at(self, i) == '\t') {
      *ptr++ = '\\';
      *ptr++ = 't';
    } else if (b_at(self, i) == '\n') {
      *ptr++ = '\\';
      *ptr++ = 'n';
    } else if (b_at(self, i) == '\r') {
      *ptr++ = '\\';
      *ptr++ = 'r';
    } else if (b_at(self, i) >= 0x20 && b_at(self, i) <= 0x7e) {
      *ptr++ = b_at(self, i);
    } else {
      sprintf(ptr, "\\x%02x", b_at(self, i));
      ptr += 4;
    }
  }

  *ptr++ = '\'';
  *ptr = '\0';

  return s;
}
