#include <test/test.h>
#include <lib/string.h>

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

void string_tests() {
  streq_basic();
}
