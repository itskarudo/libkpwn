#pragma once

#include "bytes.h"

char *url_encode(const Bytes *);
Bytes *url_decode(const char *);

char *b64e(const Bytes *);
Bytes *b64d(const char *);

char *hex(const Bytes *);
Bytes *unhex(const char *);
