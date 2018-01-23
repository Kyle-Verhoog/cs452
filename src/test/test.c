#include <assert.h>
#include <stdio.h>
#include <test/task_queue.h>
#include <test/circularbuffer.h>

int main(void) {
  printf("RUNNING UNIT TESTS\n");
  cb_sanity();
  tq_sanity();
  printf("ALL TESTS PASSED\n");
  return 0;
}

