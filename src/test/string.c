#include <test/test.h>
#include <lib/string.h>
#include <lib/parse.h>

static void streq_basic() {
  assert(!streq("woop", "notwoop"));
  assert(streq("woop", "woop"));
  assert(!streq("woopp", "woop"));
  assert(!streq("woop", "woopp"));
  assert(!streq("woof", "woop"));
  assert(!streq("1234", "wo"));
  assert(!streq("wo", "1234"));
  assert(!streq("wo", "1234555555555555555555555555555555"));
  assert(streq("ASDFASDFASDFASDFADSFASDFASDFADFS",
        "ASDFASDFASDFASDFADSFASDFASDFADFS"));
  assert(streq("", ""));
}

static void str_parse() {
  char *c = "cmd A12 B24";
  char *r;
  char buf[48];
  r = parse_str(c, buf, 48);
  assert(streq(buf, "cmd"));
  assert(r == c+3);
  r = parse_str(r, buf, 48);
  assert(streq(buf, "A12"));
  r = parse_str(r, buf, 48);
  assert(streq(buf, "B24"));

  char *s = "command      D123     F12";
  r = parse_str(s, buf, 48);
  assert(streq(buf, "command"));
  r = parse_str(r, buf, 48);
  assert(streq(buf, "D123"));
  r = parse_str(r, buf, 48);
  assert(streq(buf, "F12"));
}

void string_tests() {
  streq_basic();
  str_parse();
}
