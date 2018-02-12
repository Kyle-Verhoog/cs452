#include <user/test/create_args.h>

struct test_args {
  int arg1;
  int arg2;
};

void Test1(void *args) {
  int arg = *(int *)args;
  assert(arg == 783);
  Exit();
}

void Test2(void *args) {
  struct test_args *arg;
  arg = (struct test_args *)args;
  assert(arg->arg1 == 123);
  assert(arg->arg2 == -800);
  Exit();
}

void CreateArgsTest() {
  int arg1 = 783;
  CreateArgs(31, &Test1, &arg1);
  struct test_args arg2;
  arg2.arg1 = 123;
  arg2.arg2 = -800;
  CreateArgs(31, &Test2, &arg2);

  COMPLETE_TEST();
}
