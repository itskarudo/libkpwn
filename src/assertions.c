#include "assertions.h"
#include <stdio.h>

__attribute__((noreturn)) void __kpwn_assert_failed(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  __builtin_trap();
}
