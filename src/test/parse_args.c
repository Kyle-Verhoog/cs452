#include <test/parse_args.h>


static void parse_args_int() {
  char *buf = "123 456";
  char *args = "%d %d";
  int i, j;

  parse_args(buf, args, &i, &j);
  assert(i == 123);
  assert(j == 456);
}

static void parse_args_multi() {
  char *buf = "tr 58 14 C";
  char *args = "%s %d %d %c";
  char c, cmd[32];
  int i, j;

  parse_args(buf, args, cmd, &i, &j, &c);
  assert(streq(cmd, "tr"));
  assert(i == 58);
  assert(j == 14);
  assert(c == 'C');

  char *buf_messy = "tr    58       14            C";
  parse_args(buf_messy, args, cmd, &i, &j, &c);
  assert(streq(cmd, "tr"));
  assert(i == 58);
  assert(j == 14);
  assert(c == 'C');
}

static void parse_args_fail() {
  char *buf = "tr asdf 14 C";
  char *args = "%s %d %d %c";
  char c, cmd[32];
  int i, j, r;

  r = parse_args(buf, args, cmd, &i, &j, &c);
  assert(r == 2);

  r = parse_args("tr 58 asfd C", "%s %d %d %c", cmd, &i, &j, &c);
  assert(r == 3);

  r = parse_args("", "%d", cmd, &i, &j, &c);
  assert(r == 1);
}

void parse_args_tests() {
  parse_args_int();
  parse_args_multi();
  parse_args_fail();
}
