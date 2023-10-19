#pragma once

#include <stdbool.h>

__attribute__((noreturn)) void __kpwn_assert_failed(const char *msg);

#define __stringify_helper(x) #x
#define __stringify(x) __stringify_helper(x)

// highly inspired by serenityos
#define VERIFY(expr)                                                           \
  (__builtin_expect(!(expr), 0)                                                \
       ? __kpwn_assert_failed(#expr " at " __FILE__ ":" __stringify(__LINE__)) \
       : (void)0)

#define VERIFY_NOT_REACHED() VERIFY(false)
