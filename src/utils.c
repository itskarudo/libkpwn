#include "utils.h"
#include "kpwn.h"
#include <gc/gc.h>
#include <stdarg.h>
#include <string.h>

Bytes *flat(const Bytes *first, ...) {
  va_list args;

  va_start(args, first);

  size_t total_len = 0;
  const Bytes *tmp = first;

  while (tmp != NULL) {
    total_len += b_len(tmp);
    tmp = va_arg(args, const Bytes *);
  }

  va_end(args);

  Bytes *buffer = b_new(total_len);

  va_start(args, first);

  uint8_t *dest = b_d(buffer);

  tmp = first;

  while (tmp != NULL) {
    memcpy(dest, b_d(tmp), b_len(tmp));
    dest += b_len(tmp);
    tmp = va_arg(args, Bytes *);
  }

  va_end(args);

  return buffer;
}

Bytes *b_mul(const Bytes *bytes, size_t n) {
  Bytes *buf = b_new(b_len(bytes) * n);

  for (size_t i = 0; i < b_len(buf); i++) {
    b_at(buf, i) = b_at(bytes, i % b_len(bytes));
  }

  return buf;
}

SavedState save_state(void) {
  SavedState state = {0};
  __asm__ __volatile__(".intel_syntax noprefix;"
                       "mov %0, cs;"
                       "mov %1, ss;"
                       "mov %2, rsp;"
                       "pushf;"
                       "pop %3;"
                       ".att_syntax;"
                       : "=r"(state.cs), "=r"(state.ss), "=r"(state.sp),
                         "=r"(state.flags));
  return state;
}

Bytes * xor
    (const Bytes *first, ...) {
      va_list args;
      va_start(args, first);

      const Bytes *tmp = first;
      size_t buf_len = b_len(first);

      while (tmp != NULL) {
        if (b_len(tmp) > buf_len)
          buf_len = b_len(tmp);
        tmp = va_arg(args, const Bytes *);
      }

      va_end(args);

      Bytes *buffer = b_new(buf_len);

      memset(b_d(buffer), 0, buf_len);

      va_start(args, first);

      tmp = first;

      while (tmp != NULL) {
        for (size_t i = 0; i < buf_len; i++)
          b_at(buffer, i) ^= b_at(tmp, i % b_len(tmp));

        tmp = va_arg(args, Bytes *);
      }

      va_end(args);

      return buffer;
    }

    uint64_t posmod(int64_t i, int64_t n) {
  return (i % n + n) % n;
}
