
int mypow(int x, int y) {
  int r = 1;
  for (; y > 0; y--)
    r *= x;
  return r;
}

char *parse_c(char *str, char *c) {
  while ((*c = *(str++)) == ' ');

  if ((*c >= 'A' && *c <= 'Z') || (*c >= 'a' && *c <= 'z')) {
    return str+1;
  }
  return 0;
}

char *parse_i32(char *str, int *i) {
  char c;
  int r;
  int n = 0;
  *i = 0;

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

  r = n;
  str--;
  for (; n > 0; n--) {
    c = *(--str);
    if (c == '-')
      *i *= -1;
    else
      *i += (c-'0')*mypow(10, r-n);
  }

  return str+r;
}
