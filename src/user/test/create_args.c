#include <user/test/create_args.h>

struct test_args {
  int arg1;
  int arg2;
};

static void Test1(void *args) {
  int arg = *(int *)args;
  assert(arg == 783);
  Exit();
}

static void Test2(void *args) {
  struct test_args *arg;
  arg = (struct test_args *)args;
  assert(arg->arg1 == 123);
  assert(arg->arg2 == -800);
  Exit();
}

static void Test3(char *args) {
  assert(streq(args, "testing 123......"));
  Exit();
}

static void Test4(char *args) {
  assert(args[0] == 'a');
  assert(args[1] == 'b');
  assert(args[2] == 'c');
  assert(args[3] == 'd');
  assert(args[4] == 'e');
  assert(args[5] == '\0');
  Exit();
}

static void Test5(void *args) {
  int i;
  int j;
  i = INT_MAX;
  j = INT_MAX;
  assert(i == INT_MAX);
  assert(j == INT_MAX);
  assert(*(char *)args == '5');
  Exit();
}

static void Test6(void *args) {
  int i;
  int j;
  i = INT_MAX;
  j = INT_MAX;
  assert(i == INT_MAX);
  assert(j == INT_MAX);
  int *ii;
  ii = *(int **)args;
  assert(*ii == INT_MAX-1);
  Exit();
}

void CreateArgsTest() {
  int arg1 = 783;
  CreateArgs(31, &Test1, &arg1, sizeof(arg1));
  struct test_args arg2;
  arg2.arg1 = 123;
  arg2.arg2 = -800;
  CreateArgs(31, &Test2, &arg2, sizeof(arg2));

  char *str = "testing 123......";
  CreateArgs(31, &Test3, str, strlen(str)+1);

  char *str2 = "abcde";
  assert(strlen(str2)+1 == 6);
  CreateArgs(31, &Test4, str2, strlen(str2)+1);

  char c = '5';
  CreateArgs(31, &Test5, &c, sizeof(c));

  int j = INT_MAX-1;
  int *i = &j;
  CreateArgs(31, &Test6, &i, sizeof(&i));

  COMPLETE_TEST();
}
