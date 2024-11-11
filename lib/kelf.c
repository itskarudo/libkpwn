#include "kelf.h"
#include <elf.h>
#include <fcntl.h>
#include <gc/gc.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utarray.h>
#include <uthash.h>

#undef uthash_malloc
#undef uthash_free

#define uthash_malloc(sz) GC_MALLOC_ATOMIC(sz)
#define uthash_free(ptr, sz) GC_FREE(ptr)

struct _ELF {
  uint64_t address;
  uint8_t *data;
  struct KeyLongMap *syms;
  struct KeyLongMap *got;
  UT_array *sections;
  bool statically_linked;
};

struct KeyLongMap {
  const char *name;
  uint64_t value;
  UT_hash_handle hh;
};

struct ELFSection {
  const char *name;
  uint32_t type;
  uint32_t link;
  uint64_t offset;
  uint64_t size;
};

UT_icd section_icd = {sizeof(struct ELFSection), NULL, NULL, NULL};

// TODO: bound checking, a lot of bound checking

// some preprocessor abuse will not hurt anybody

#define ELF_N_GENERATE_PARSERS(n)                                              \
                                                                               \
  static void elf##n##_parse_sections(ELF *self) {                             \
    Elf##n##_Ehdr *hdr = (Elf##n##_Ehdr *)self->data;                          \
    Elf##n##_Shdr *shdr = (Elf##n##_Shdr *)(self->data + hdr->e_shoff);        \
                                                                               \
    size_t shnum = hdr->e_shnum;                                               \
    Elf##n##_Shdr *sh_strtab = &shdr[hdr->e_shstrndx];                         \
    uint8_t *sh_strtab_p = self->data + sh_strtab->sh_offset;                  \
                                                                               \
    for (size_t i = 0; i < shnum; i++) {                                       \
      struct ELFSection section;                                               \
      section.name = (const char *)(sh_strtab_p + shdr[i].sh_name);            \
      section.type = shdr[i].sh_type;                                          \
      section.link = shdr[i].sh_link;                                          \
      section.offset = shdr[i].sh_offset;                                      \
      section.size = shdr[i].sh_size;                                          \
      utarray_push_back(self->sections, &section);                             \
    }                                                                          \
  }                                                                            \
                                                                               \
  static void elf##n##_parse_symbols(ELF *self) {                              \
                                                                               \
    uint8_t *sh_strtab = NULL;                                                 \
    uint8_t *sh_dynstr = NULL;                                                 \
                                                                               \
    struct ELFSection *s = NULL;                                               \
    while ((s = utarray_next(self->sections, s)) != NULL) {                    \
                                                                               \
      if (sh_strtab != NULL && sh_dynstr != NULL)                              \
        break;                                                                 \
                                                                               \
      if (s->type == SHT_STRTAB && !strcmp(s->name, ".dynstr")) {              \
        sh_dynstr = self->data + s->offset;                                    \
      }                                                                        \
                                                                               \
      if (s->type == SHT_STRTAB && !strcmp(s->name, ".strtab")) {              \
        sh_strtab = self->data + s->offset;                                    \
      }                                                                        \
    }                                                                          \
                                                                               \
    s = NULL;                                                                  \
    while ((s = utarray_next(self->sections, s)) != NULL) {                    \
                                                                               \
      if (s->type != SHT_SYMTAB && s->type != SHT_DYNSYM)                      \
        continue;                                                              \
                                                                               \
      size_t n_syms = s->size / sizeof(Elf##n##_Sym);                          \
      Elf##n##_Sym *sym_data = (Elf##n##_Sym *)(self->data + s->offset);       \
                                                                               \
      for (size_t i = 0; i < n_syms; i++) {                                    \
                                                                               \
        const char *name;                                                      \
                                                                               \
        if (s->type == SHT_SYMTAB) {                                           \
          name = (const char *)(sh_strtab + sym_data[i].st_name);              \
        } else {                                                               \
          name = (const char *)(sh_dynstr + sym_data[i].st_name);              \
        }                                                                      \
                                                                               \
        if (!strcmp(name, ""))                                                 \
          continue;                                                            \
                                                                               \
        struct KeyLongMap *sym = GC_MALLOC_ATOMIC(sizeof(struct KeyLongMap));  \
        sym->value = sym_data[i].st_value;                                     \
        sym->name = name;                                                      \
                                                                               \
        HASH_ADD_STR(self->syms, name, sym);                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  static void elf##n##_parse_got(ELF *self) {                                  \
                                                                               \
    uint8_t *dynstr = NULL;                                                    \
    struct ELFSection *s = NULL;                                               \
    while ((s = utarray_next(self->sections, s)) != NULL) {                    \
      if (s->type == SHT_STRTAB && !strcmp(s->name, ".dynstr")) {              \
        dynstr = self->data + s->offset;                                       \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    s = NULL;                                                                  \
    while ((s = utarray_next(self->sections, s)) != NULL) {                    \
      if (s->type != SHT_REL && s->type != SHT_RELA)                           \
        continue;                                                              \
                                                                               \
      if (s->link == SHN_UNDEF)                                                \
        continue;                                                              \
                                                                               \
      struct ELFSection *symbols = utarray_eltptr(self->sections, s->link);    \
                                                                               \
      size_t item_size =                                                       \
          s->type == SHT_REL ? sizeof(Elf##n##_Rel) : sizeof(Elf##n##_Rela);   \
                                                                               \
      size_t n_rels = s->size / item_size;                                     \
                                                                               \
      uint8_t *rels = self->data + s->offset;                                  \
                                                                               \
      for (size_t i = 0; i < n_rels * item_size; i += item_size) {             \
        Elf##n##_Rel *rel = (Elf##n##_Rel *)(rels + i);                        \
                                                                               \
        size_t sym_idx = ELF##n##_R_SYM(rel->r_info);                          \
        Elf##n##_Sym *sym =                                                    \
            &((Elf##n##_Sym *)(self->data + symbols->offset))[sym_idx];        \
                                                                               \
        char *name = (char *)(dynstr + sym->st_name);                          \
                                                                               \
        if (!strcmp(name, ""))                                                 \
          continue;                                                            \
                                                                               \
        struct KeyLongMap *got_entry =                                         \
            GC_MALLOC_ATOMIC(sizeof(struct KeyLongMap));                       \
        got_entry->name = name;                                                \
        got_entry->value = rel->r_offset;                                      \
                                                                               \
        HASH_ADD_STR(self->got, name, got_entry);                              \
      }                                                                        \
    }                                                                          \
  }

ELF_N_GENERATE_PARSERS(32)
ELF_N_GENERATE_PARSERS(64)

static inline void elf_parse_sections(ELF *self, uint8_t binary_class) {
  if (binary_class == ELFCLASS32)
    elf32_parse_sections(self);
  else
    elf64_parse_sections(self);
}

static inline void elf_parse_symbols(ELF *self, uint8_t binary_class) {
  if (binary_class == ELFCLASS32)
    elf32_parse_symbols(self);
  else
    elf64_parse_symbols(self);
}

static inline void elf_parse_got(ELF *self, uint8_t binary_class) {
  if (binary_class == ELFCLASS32)
    elf32_parse_got(self);
  else
    elf64_parse_got(self);
}

void elf_finalizer(ELF *self, void *user_data) {
  (void)user_data;
  utarray_free(self->sections);
}

ELF *elf_new(const char *filename) {

  ELF *self = GC_MALLOC_ATOMIC(sizeof(ELF));
  if (self == NULL)
    return NULL;

  self->syms = NULL;

  int fd = open(filename, O_RDONLY);
  if (fd < 0)
    return NULL;

  struct stat st;
  stat(filename, &st);

  uint8_t *data = GC_MALLOC_ATOMIC(st.st_size);

  if (data == NULL) {
    close(fd);
    return NULL;
  }

  read(fd, data, st.st_size);
  close(fd);

  self->data = data;

  Elf64_Ehdr *hdr = (Elf64_Ehdr *)data;

  uint8_t binary_class = hdr->e_ident[EI_CLASS];

  utarray_new(self->sections, &section_icd);

  GC_register_finalizer(self, (GC_finalization_proc)elf_finalizer, NULL, NULL,
                        NULL);

  // TODO: set this dynamically for binaries with PIE disabled
  self->address = 0;

  self->statically_linked = hdr->e_type == ET_EXEC;

  elf_parse_sections(self, binary_class);

  elf_parse_symbols(self, binary_class);

  if (!self->statically_linked) {
    elf_parse_got(self, binary_class);
  }

  return self;
}

uint64_t elf_sym(ELF *self, const char *name) {
  struct KeyLongMap *s;
  HASH_FIND_STR(self->syms, name, s);
  return s ? self->address + s->value : 0;
}

uint64_t elf_got(ELF *self, const char *name) {
  struct KeyLongMap *s;
  HASH_FIND_STR(self->got, name, s);
  return s ? self->address + s->value : 0;
}
