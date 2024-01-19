#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct _ELF ELF;

#ifdef __cplusplus
extern "C" {
#endif

ELF *elf_new(const char *filename);
uint64_t elf_sym(ELF *, const char *name);
uint64_t elf_got(ELF *, const char *name);

#ifdef __cplusplus
}
#endif
