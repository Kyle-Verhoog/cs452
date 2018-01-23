#include <assert.h>
#include <stdio.h>
#include <test/task_queue.h>

int main(void) {
  printf("RUNNING UNIT TESTS\n");
  tq_sanity();
  printf("ALL TESTS PASSED\n");
  return 0;
}

