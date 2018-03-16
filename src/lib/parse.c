
int mypow(int x, int y) {
  int r = 1;
  for (; y > 0; y--)
    r *= x;
  return r;
}

int parse_str(char *str, char *buf, int buf_len) {
  char c, *s;
  int len;
  len = 0;
  s = str;
  while ((c = *(str++)) == ' ');
  str--;
  while ((c = *str++) != ' ' && c != '\0') {
    *buf++ = c;
    ++len;
    if (len > buf_len)
      return 0;
  }
  *buf++ = '\0';
  return str - s;
}


int parse_c(char *str, char *c) {
  char *s;
  s = str;
  while ((*c = *(str++)) == ' ');

  if ((*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z')) {
    return str-s+1;
  }
  return 0;
}

int parse_ui32(char *str, int *i) {
  char *s, c;
  int r, n, off;
  *i = n = off = 0;
  s = str;

  // skip all leading whitespace
  while ((c = *(str++)) == ' ');
  str--;

  // iterate to end of number
  while ((c = *(str++)) != '\0' &&
        ((c <= '9' && c >= '0') ||
         (c == '-' && (*str <= '9' && *str >= '0'))))
    n++;

  // we didn't get a number
  if (n < 1) return 0;

  off = str - s;
  r = n;
  str--;
  for (; n > 0; n--) {
    c = *(--str);
    if (c == '-')
      *i *= -1;
    else
      *i += (c-'0')*mypow(10, r-n);
  }

  return off;
}
