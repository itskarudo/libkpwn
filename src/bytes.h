#pragma once

#include <stddef.h>
#include <stdint.h>
typedef struct {
  size_t _len;
  uint8_t _data[];
} Bytes;

Bytes *b_new(size_t n);
Bytes *b_new_v(const uint8_t *, size_t n);
Bytes *b_slice(const Bytes *, size_t start, size_t end);
int b_cmp(const Bytes *, const Bytes *);
const char *b_s(const Bytes *);

#define b(v) (b_new_v((uint8_t *)v, sizeof(v) - 1))
#define b_d(v) (v->_data)
#define b_at(v, i) (v->_data[i])
#define b_len(v) (v->_len)

#define p8(v)                                                                  \
  ({                                                                           \
    uint8_t _tmp = v;                                                          \
    b_new_v(&_tmp, 1);                                                         \
  })

#define p16(v)                                                                 \
  ({                                                                           \
    uint16_t _tmp = v;                                                         \
    b_new_v((uint8_t *)&_tmp, 2);                                              \
  })

#define p32(v)                                                                 \
  ({                                                                           \
    uint32_t _tmp = v;                                                         \
    b_new_v((uint8_t *)&_tmp, 4);                                              \
  })

#define p64(v)                                                                 \
  ({                                                                           \
    uint64_t _tmp = v;                                                         \
    b_new_v((uint8_t *)&_tmp, 8);                                              \
  })

#define u8(v) (*(uint8_t *)v->_data)
#define u16(v) (*(uint16_t *)v->_data)
#define u32(v) (*(uint32_t *)v->_data)
#define u64(v) (*(uint64_t *)v->_data)
