#include <test/buffer_pack.h>

void pack_c() {
  char buf[30];
  int offset;
  offset = 0;
  offset += buf_pack_c(buf + offset, 'a');
  assert(buf[0] == 'a');
  assert(offset == 1);

  offset += buf_pack_c(buf + offset, 'b');
  assert(buf[1] == 'b');
  assert(offset == 2);

  offset += buf_pack_c(buf + offset, 'c');
  assert(buf[0] == 'a');
  assert(buf[1] == 'b');
  assert(buf[2] == 'c');
  assert(offset == 3);
}

void pack_i32() {
  char buf[100];
  int offset;
  offset = 0;

  offset += buf_pack_i32(buf + offset, 32);
  assert(buf[0] == '3');
  assert(buf[1] == '2');
  assert(offset == 2);

  offset += buf_pack_i32(buf + offset, -435);
  assert(buf[2] == '-');
  assert(buf[3] == '4');
  assert(buf[4] == '3');
  assert(buf[5] == '5');
  assert(offset == 6);

  offset += buf_pack_i32(buf + offset, -2147483648);
  assert(buf[0] == '3');
  assert(buf[1] == '2');
  assert(buf[2] == '-');
  assert(buf[3] == '4');
  assert(buf[4] == '3');
  assert(buf[5] == '5');
  assert(buf[6] == '-');
  assert(buf[7] == '2');
  assert(buf[8] == '1');
  assert(buf[9] == '4');
  assert(buf[10] == '7');
  assert(buf[11] == '4');
  assert(buf[12] == '8');
  assert(buf[13] == '3');
  assert(buf[14] == '6');
  assert(buf[15] == '4');
  assert(buf[16] == '8');
  assert(offset == 17);
}

void pack_str() {
  char buf[100];
  int offset;
  offset = 0;

  offset += buf_pack_str(buf + offset, "asdf");
  assert(buf[0] == 'a');
  assert(buf[1] == 's');
  assert(buf[2] == 'd');
  assert(buf[3] == 'f');
  assert(offset == 4);

  offset += buf_pack_str(buf + offset, "asdf");
  assert(buf[4] == 'a');
  assert(buf[5] == 's');
  assert(buf[6] == 'd');
  assert(buf[7] == 'f');
  assert(offset == 8);
}

void pack_fmt() {
  char buf[100];
  int offset;
  offset = 0;

  offset += buf_pack_f(buf + offset, "asdf");
  assert(buf[0] == 'a');
  assert(buf[1] == 's');
  assert(buf[2] == 'd');
  assert(buf[3] == 'f');
  assert(offset == 4);

  offset = 0;
  offset += buf_pack_f(buf + offset, "a%c%d%d%s", ';', -34, 0, "test");
  assert(offset > 0);
  assert(buf[0] == 'a');
  assert(buf[1] == ';');
  assert(buf[2] == '-');
  assert(buf[3] == '3');
  assert(buf[4] == '4');
  assert(buf[5] == '0');
  assert(buf[6] == 't');
  assert(buf[7] == 'e');
  assert(buf[8] == 's');
  assert(buf[9] == 't');
  assert(offset == 10);
}

void buffer_pack_tests() {
  pack_c();
  pack_i32();
  pack_str();
  pack_fmt();
}
