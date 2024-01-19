#include "encoding.h"
#include "assertions.h"
#include "kpwn.h"
#include <ctype.h>
#include <gc/gc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *url_encode(const Bytes *str) {
  char *buf = GC_MALLOC_ATOMIC(b_len(str) * 3 + 1), *pbuf = buf;
  size_t pstr = 0;

  while (pstr < b_len(str)) {
    if (isalnum(b_at(str, pstr)) || b_at(str, pstr) == '-' ||
        b_at(str, pstr) == '_' || b_at(str, pstr) == '.' ||
        b_at(str, pstr) == '~') {

      *pbuf++ = b_at(str, pstr);

    } else if (b_at(str, pstr) == ' ') {
      *pbuf++ = '+';
    } else {
      sprintf(pbuf, "%%%02X", (uint8_t)b_at(str, pstr));
      pbuf += 3;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

Bytes *url_decode(const char *str) {

  size_t len = strlen(str);
  Bytes *buf = b_new(len);

  size_t pstr = 0;
  size_t pbts = 0;

  while (pstr < len) {
    if (str[pstr] == '+') {
      b_at(buf, pbts++) = ' ';
    } else if (str[pstr] == '%') {
      char hex[3] = {str[pstr + 1], str[pstr + 2], '\0'};
      b_at(buf, pbts++) = (uint8_t)strtol(hex, NULL, 16);
      pstr += 2;
    } else {
      b_at(buf, pbts++) = str[pstr];
    }
    pstr++;
  }
  return buf;
}

static const char base64_map[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

char *b64e(const Bytes *bytes) {

  size_t counts = 0;
  char buffer[3];
  char *cipher = GC_MALLOC_ATOMIC(b_len(bytes) * 4 / 3 + 4);
  int i = 0, c = 0;

  for (i = 0; b_at(bytes, i) != '\0'; i++) {
    buffer[counts++] = b_at(bytes, i);
    if (counts == 3) {
      cipher[c++] = base64_map[buffer[0] >> 2];
      cipher[c++] = base64_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
      cipher[c++] = base64_map[((buffer[1] & 0x0f) << 2) + (buffer[2] >> 6)];
      cipher[c++] = base64_map[buffer[2] & 0x3f];
      counts = 0;
    }
  }

  if (counts > 0) {
    cipher[c++] = base64_map[buffer[0] >> 2];
    if (counts == 1) {
      cipher[c++] = base64_map[(buffer[0] & 0x03) << 4];
      cipher[c++] = '=';
    } else { // if counts == 2
      cipher[c++] = base64_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
      cipher[c++] = base64_map[(buffer[1] & 0x0f) << 2];
    }
    cipher[c++] = '=';
  }

  cipher[c] = '\0'; /* string padding character */
  return cipher;
}

Bytes *b64d(const char *str) {

  size_t counts = 0;
  char buffer[4];
  Bytes *plain = GC_MALLOC_ATOMIC(strlen(str) * 3 / 4);
  int i = 0, p = 0;

  for (i = 0; str[i] != '\0'; i++) {
    size_t k;
    for (k = 0; k < 64 && base64_map[k] != str[i]; k++)
      ;
    buffer[counts++] = k;
    if (counts == 4) {
      b_at(plain, p++) = (buffer[0] << 2) + (buffer[1] >> 4);
      if (buffer[2] != 64)
        b_at(plain, p++) = (buffer[1] << 4) + (buffer[2] >> 2);
      if (buffer[3] != 64)
        b_at(plain, p++) = (buffer[2] << 6) + buffer[3];
      counts = 0;
    }
  }

  b_len(plain) = p;
  return plain;
}

static const char *hex_map = "0123456789abcdef";

char *hex(const Bytes *bytes) {

  char *buffer = GC_MALLOC_ATOMIC(b_len(bytes) * 2 + 1);

  for (size_t i = 0; i < b_len(bytes); i++) {
    buffer[i * 2] = hex_map[(b_at(bytes, i) >> 4) & 0xF];
    buffer[i * 2 + 1] = hex_map[b_at(bytes, i) & 0xF];
  }

  buffer[b_len(bytes) * 2] = 0;
  return buffer;
}

Bytes *unhex(const char *hex) {
  Bytes *self = b_new(strlen(hex) / 2);

  for (size_t i = 0; i < b_len(self); i++) {
    char c = hex[i * 2];
    char d = hex[i * 2 + 1];

    if (c >= '0' && c <= '9')
      c -= '0';
    else if (c >= 'a' && c <= 'f')
      c -= 'a' - 10;
    else if (c >= 'A' && c <= 'F')
      c -= 'A' - 10;
    else
      VERIFY_NOT_REACHED();

    if (d >= '0' && d <= '9')
      d -= '0';
    else if (d >= 'a' && d <= 'f')
      d -= 'a' - 10;
    else if (d >= 'A' && d <= 'F')
      d -= 'A' - 10;
    else
      VERIFY_NOT_REACHED();

    b_at(self, i) = (c << 4) | d;
  }

  return self;
}
