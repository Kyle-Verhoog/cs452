#include <lib/buffer_pack.h>

int buf_pack_c(char *buf, char c) {
  *buf = c;
  return 1;
}

int buf_pack_ui32(char *buf, unsigned int i, unsigned int base) {
  int n = 0;
  int dgt;
  char c;
  unsigned int d;
  int offset;

  d = 1;
  offset = 0;

  while( (i / d) >= base ) d *= base;
  while( d != 0 ) {
    dgt = i / d;
    i %= d;
    d /= base;
    if(n || dgt > 0 || d == 0) {
      c = dgt + (dgt < 10 ? '0' : 'a' - 10);
      offset += buf_pack_c(buf + offset, c);
      ++n;
    }
  }
  return offset;
}

int buf_pack_i32(char *buf, int i) {
  int offset;
  offset = 0;

  if (i < 0) {
    i = -i;
    offset += buf_pack_c(buf + offset, '-');
  }
  offset += buf_pack_ui32(buf + offset, i, 10);
  return offset;
}

int buf_pack_str(char *buf, char *str) {
  char c;
  int offset;
  offset = 0;

  while ((c = *(str++))) {
    offset += buf_pack_c(buf + offset, c);
  }
  return offset;
}

int buf_pack_fmt(char *buf, char *fmt, va_list va) {
  int offset;
  char c;

  offset = 0;

  while ((c = *(fmt++))) {
    if (c != '%') {
      offset += buf_pack_c(buf + offset, c);
    }
    else {
      c = *(fmt++);
      switch (c) {
        case 'c':
          offset += buf_pack_c(buf + offset, va_arg(va, int));
          break;
        case 'u':
          offset += buf_pack_ui32(buf + offset, va_arg(va, unsigned int), 10);
          break;
        case 'd':
          offset += buf_pack_i32(buf + offset, va_arg(va, int));
          break;
        case 's':
          offset += buf_pack_str(buf + offset, va_arg(va, char *));
          break;
        default:
          return -1;
          break;
      }
    }
  }
  return offset;
}


int buf_pack_f(char *buf, char *fmt, ...) {
  int offset;
  va_list va;

  offset = 0;

  va_start(va, fmt);
  offset += buf_pack_fmt(buf + offset, fmt, va);
  va_end(va);

  return offset;
}
