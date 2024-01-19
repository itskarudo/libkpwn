#pragma once

#include "bytes.h"

#ifdef __cplusplus
extern "C" {
#endif
char *url_encode(const Bytes *);
Bytes *url_decode(const char *);

char *b64e(const Bytes *);
Bytes *b64d(const char *);

char *hex(const Bytes *);
Bytes *unhex(const char *);

#ifdef __cplusplus
}
#endif
