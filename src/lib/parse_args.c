#include <lib/parse_args.h>
#include <assert.h>


int parse_args_va(char *buf, char *args, int *boff, int narg, va_list va) {
  char c, *str, *ch;
  int r, *i;

  while ((c = *(args++))) {
    if (c != '%') {
    }
    else {
      c = *(args++);
      switch (c) {
        case 'd':
          i = va_arg(va, int *);
          if ((r = parse_ui32(buf + *boff, i)) <= 0)
            return narg;
          *boff += r;
          break;
        case 's':
          str = va_arg(va, char *);
          if ((r = parse_str(buf + *boff, str, sizeof(str))) <= 0)
            return narg;
          *boff += r;
          break;
        case 'c':
          ch = va_arg(va, char *);
          if ((r = parse_c(buf + *boff, ch)) <= 0)
            return narg;
          *boff += r;
          break;
        default:
          return -1;
          break;
      }
      narg++;
    }
  }
  return 0;
}

int parse_args(char *buf, char *args, ...) {
  int boff, r, narg;
  va_list va;

  boff = 0;
  narg = 1;

  va_start(va, args);
  r = parse_args_va(buf, args, &boff, narg, va);
  if (r) {
    return r;
  }
  va_end(va);

  return 0;
}
