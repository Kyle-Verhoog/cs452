#include <test/test.h>
#include <test/circularbuffer.h>
#include <test/task_queue.h>
#include <test/priority_queue.h>
#include <test/nameserver.h>
#include <test/clockserver_queue.h>

int main(void) {
  PRINTF("\n");
  PRINTF("+--------------------------+\n");
  PRINTF("|    RUNNING UNIT TESTS    |\n");
  PRINTF("+----------------------+---+\n");
  
  PTEST_START("circularbuffer");
  circular_buffer_tests();
  PTEST_END();
  
  PTEST_START("task_queue");
  task_queue_tests();
  PTEST_END();
  
  PTEST_START("priority_queue");
  priority_queue_tests();
  PTEST_END();

  PTEST_START("nameserver");
  nameserver_tests();
  PTEST_END();

  PTEST_START("cs_queue");
  clockserver_tests();
  PTEST_END();
  PRINTF("+----------------------+---+\n");
  PRINTF("|    ALL TESTS PASSED ☻    |\n");
  PRINTF("+--------------------------+\n");

  return 0;
}

