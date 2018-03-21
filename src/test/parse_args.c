#include <test/parse_args.h>

static void parse_args_int() {
  char *buf = "3 4";
  char *args = "%d %d";
  int i, j, r;

  r = parse_args(buf, args, &i, &j);
  assert(r == 0);
  assert(i == 3);
  assert(j == 4);
}

static void parse_args_multi() {
  char *buf = "tr 58 14 C";
  char *args = "%s %d %d %c";
  char c, cmd[32];
  int i, j, r;

  r = parse_args(buf, args, cmd, sizeof(cmd), &i, &j, &c);
  assert (r == 0);
  assert(streq(cmd, "tr"));
  assert(i == 58);
  assert(j == 14);
  assert(c == 'C');

  char *buf_messy = "tr    58       14            C";
  r = parse_args(buf_messy, args, cmd, sizeof(cmd), &i, &j, &c);
  assert(r == 0);
  assert(streq(cmd, "tr"));
  assert(i == 58);
  assert(j == 14);
  assert(c == 'C');
}

static void parse_str_str() {
  int i, r;
  char *buf = "dr 24 A1 A2";
  char *args = "%s %d %s %s";
  char c, cmd[32];
  char node1[5], node2[5];
  r = parse_args(buf, args, cmd, sizeof(cmd), &i, node1, sizeof(node1), node2, sizeof(node2));
  assert(streq(cmd, "dr"));
  assert(streq(node1, "A1"));
  assert(streq(node2, "A2"));
}

static void parse_args_fail() {
  char *buf = "tr asdf 14 C";
  char *args = "%s %d %d %c";
  char c, cmd[32];
  int i, j, r;

  r = parse_args(buf, args, cmd, sizeof(cmd), &i, &j, &c);
  assert(r == 2);

  r = parse_args("tr 58 asfd C", "%s %d %d %c", cmd, sizeof(cmd), &i, &j, &c);
  assert(r == 3);

  r = parse_args("", "%d", cmd, &i, &j, &c);
  assert(r == 1);
}

static void parse_cmd() {
  char *raw_cmd = "tr 58 14";
  char cmd[32];
  int r;

  r = parse_str(raw_cmd, cmd, sizeof(cmd));
  assert(streq(cmd, "tr"));
}

void parse_args_tests() {
  parse_args_int();
  parse_args_multi();
  parse_args_fail();
  parse_str_str();
  parse_cmd();
}
