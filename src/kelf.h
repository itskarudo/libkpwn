#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <utarray.h>

typedef struct {
  uint64_t address;
  uint8_t *data;
  struct KeyLongMap *syms;
  struct KeyLongMap *got;
  UT_array *sections;
  bool statically_linked;
} ELF;

ELF *elf_new(const char *filename);
uint64_t elf_sym(ELF *, const char *name);
uint64_t elf_got(ELF *, const char *name);
