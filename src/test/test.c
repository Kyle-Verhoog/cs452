#include <assert.h>
#include <stdio.h>
#include <test/circularbuffer.h>
#include <test/task_queue.h>
#include <test/priority_queue.h>

int main(void) {
  printf("\n=== RUNNING UNIT TESTS === \n");
  circular_buffer_tests();
  task_queue_tests();
  priority_queue_tests();
  printf("✓✓✓ ALL TESTS PASSED ✓✓✓\n\n");
  return 0;
}

