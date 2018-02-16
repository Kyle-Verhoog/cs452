#include <user/uio.h>

int putc(tid_t ios_tid, char c) {
  int r;
  assert(ios_tid > 0);

  IOServerReq req;
  char rep;

  req.type = IO_PUTC;
  req.msg = c;
  req.len = 1;

  // SANITY();
  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  // SANITY();
  assert(r == 0);
  return 0;
}

int putx(tid_t ios_tid, char c) {
  char chh, chl;

  chh = c2x(c / 16);
  chl = c2x(c % 16);
  putc(ios_tid, chh);
  return putc(ios_tid, chl);
}

int putr(tid_t ios_tid, unsigned int reg) {
  int byte;
  volatile char *ch = (char *) &reg;

  for(byte = 3; byte >= 0; byte--) putx(ios_tid, ch[byte]);
  return putc(ios_tid, ' ');
}

int putstr(tid_t ios_tid, const volatile char *str) {
  while(*str) {
    if(putc(ios_tid, *str) < 0) return -1;
    str++;
  }
  return 0;
}

void putw(tid_t ios_tid, int n, char fc, const char *bf) {
  char ch;
  const char *p = bf;

  while(*p++ && n > 0) n--;
  while(n-- > 0) putc(ios_tid, fc);
  while((ch = *bf++)) putc(ios_tid, ch);
}

int a2d(char ch) {
  if(ch >= '0' && ch <= '9') return ch - '0';
  if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
  return -1;
}

char a2i(char ch, const volatile char **src, int base, int *nump) {
  int num, digit;
  const volatile char *p;

  p = *src;
  num = 0;
  while((digit = a2d(ch)) >= 0) {
    if (digit > base) break;
    num = num*base + digit;
    ch = *p++;
  }
  *src = p;
  *nump = num;
  return ch;
}

void uint2a(unsigned int num, unsigned int base, volatile char *bf) {
  int n = 0;
  int dgt;
  unsigned int d = 1;

  while((num / d) >= base) d *= base;
  while(d != 0) {
    dgt = num / d;
    num %= d;
    d /= base;
    if(n || dgt > 0 || d == 0) {
      *bf++ = dgt + (dgt < 10 ? '0' : 'a' - 10);
      ++n;
    }
  }
  *bf = 0;
}

void int2a(int num, volatile char *bf) {
  if(num < 0) {
    num = -num;
    *bf++ = '-';
  }
  uint2a(num, 10, bf);
}

void format(tid_t ios_tid, const volatile char *fmt, va_list va) {
  char bf[12];
  char ch, lz;
  int w;


  while ((ch = *(fmt++))) {
    if (ch != '%')
      putc(ios_tid, ch);
    else {
      lz = 0;
      w = 0;
      ch = *(fmt++);
      switch (ch) {
        case '0':
          lz = 1;
          ch = *(fmt++);
          break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          ch = a2i(ch, &fmt, 10, &w);
          break;
      }
      switch(ch) {
        case 0:
          return;
        case 'c':
          putc(ios_tid, va_arg(va, char));
          break;
        case 's':
          putw(ios_tid, w, 0, va_arg(va, char*));
          break;
        case 'u':
          uint2a(va_arg(va, unsigned int), 10, bf);
          putw(ios_tid, w, lz, bf);
          break;
        case 'd':
          int2a(va_arg(va, int), bf);
          putw(ios_tid, w, lz, bf);
          break;
        case 'x':
          uint2a(va_arg(va, unsigned int), 16, bf);
          putw(ios_tid, w, lz, bf);
          break;
        case '%':
          putc(ios_tid, ch);
          break;
      }
    }
  }
}

void print(tid_t ios_tid, const volatile char *fmt, ...) {
  va_list va;

  va_start(va,fmt);
  format(ios_tid, fmt, va);
  va_end(va);
}

