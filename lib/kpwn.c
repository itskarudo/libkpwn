#include <gc/gc.h>

static __attribute__((constructor)) void kpwn_init(void) { GC_INIT(); }
