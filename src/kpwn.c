#include "kpwn.h"
#include <gc/gc.h>
#include <stdlib.h>

static __attribute__((constructor)) void kpwn_init(void) { GC_INIT(); }
