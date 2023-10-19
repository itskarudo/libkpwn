#pragma once
#include "bytes.h"

typedef struct {
  uint64_t cs;
  uint64_t ss;
  uint64_t sp;
  uint64_t flags;
} SavedState;

SavedState save_state(void);

Bytes *flat(const Bytes *, ...);
Bytes *b_mul(const Bytes *, size_t n);
Bytes * xor (const Bytes *, ...);
uint64_t posmod(int64_t i, int64_t n);
