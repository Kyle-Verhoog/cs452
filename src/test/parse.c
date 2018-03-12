#include <test/parse.h>

static void parse_c_test() {
  char c, *str = "t r a i n z";
  int r;

  r = parse_c(str, &c);
  assert(r == 2);
  assert(c == 't');

  r += parse_c(str+r, &c);
  assert(c == 'r');
  assert(r == 4);

  r += parse_c(str+r, &c);
  assert(c == 'a');
  assert(r == 6);

  r += parse_c(str+r, &c);
  assert(c == 'i');
  assert(r == 8);

  r += parse_c(str+r, &c);
  assert(c == 'n');
  assert(r == 10);

  r += parse_c(str+r, &c);
  assert(c == 'z');
  assert(r == 12);

  r = parse_c(str+11, &c);
  assert(r == 0);
}

static void parse_str_test() {
  char *c = "cmd A12 B24";
  int r;
  char buf[48];
  r = parse_str(c, buf, 48);
  assert(streq(buf, "cmd"));
  assert(r == 4);
  r += parse_str(c+r, buf, 48);
  assert(r == 8);
  assert(streq(buf, "A12"));
  r += parse_str(c+r, buf, 48);
  assert(r == 12);
  assert(streq(buf, "B24"));

  char *s = "command      D123     F12";
  r = parse_str(s, buf, 48);
  assert(streq(buf, "command"));
  r += parse_str(s+r, buf, 48);
  assert(streq(buf, "D123"));
  r += parse_str(s+r, buf, 48);
  assert(streq(buf, "F12"));
}

static void parse_ui32_test() {
  char *str = "123";
  int i, r;

  r = parse_ui32(str, &i);
  assert(i == 123);
  assert(r == 4);

  char *str1 = "lfjsa";
  r = parse_ui32(str1, &i);
  assert(r == 0);

  char *args = "123 456 789";
  r = parse_ui32(args, &i);
  assert(r == 4);
  assert(i == 123);
  r += parse_ui32(args+r, &i);
  assert(r == 8);
  assert(i == 456);
  r += parse_ui32(args+r, &i);
  assert(r == 12);
  assert(i == 789);
}

static void parse_cmd() {
  int r;
  char buf[32];
  char *cmd1 = "cmd arg1 arg2";

  r = parse_str(cmd1, buf, sizeof(buf));
  assert(r == 4);
  assert(streq(buf, "cmd"));
  r += parse_str(cmd1+r, buf, sizeof(buf));
  assert(r == 9);
  assert(streq(buf, "arg1"));
  r += parse_str(cmd1+r, buf, sizeof(buf));
  assert(r == 14);
  assert(streq(buf, "arg2"));

  char *cmd2 = "stop   1  A12 9324";
  int i;
  r = parse_str(cmd2, buf, sizeof(buf));
  assert(r == 5);
  assert(streq(buf, "stop"));
  r += parse_ui32(cmd2+r, &i);
  assert(r == 9);
  assert(i == 1);
  r += parse_str(cmd2+r, buf, sizeof(buf));
  assert(streq(buf, "A12"));
  assert(r == 14);
  r += parse_ui32(cmd2+r, &i);
  assert(i == 9324);
  assert(r == 19);
}

void parse_tests() {
  parse_ui32_test();
  parse_c_test();
  parse_str_test();
  parse_cmd();
}
