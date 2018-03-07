#include <test/test.h>
#include <test/task_queue.h>
#include <test/priority_queue.h>
#include <test/nameserver.h>
#include <test/clockserver_queue.h>
#include <test/circular_buffer.h>
#include <test/buffer_pack.h>
#include <test/ioserver_buffer.h>
#include <test/terminal.h>
#include <test/train/priority_queue.h>
#include <test/train/path_finding.h>
#include <test/string.h>

int main(void) {
  PRINTF("\n");
  PRINTF("+----------------------+---+\n");
  PRINTF("|  RUNNING UNIT TESTS  | ☐ |\n");
  PRINTF("+----------------------+---+\n");

  PTEST_START("lib/buffer_pack");
  buffer_pack_tests();
  PTEST_END();

  PTEST_START("lib/string");
  string_tests();
  PTEST_END();

  PTEST_START("circular_buffer");
  cbuf_tests();
  PTEST_END();

  PTEST_START("lib/cs_queue");
  clockserver_tests();
  PTEST_END();

  PTEST_START("ioserver_buffer");
  ioserver_buffer_tests();
  PTEST_END();

  PTEST_START("nameserver");
  nameserver_tests();
  PTEST_END();

  PTEST_START("lib/task_queue");
  task_queue_tests();
  PTEST_END();

  PTEST_START("task/priority_queue");
  priority_queue_tests();
  PTEST_END();

  PTEST_START("lib/terminal");
  terminal_tests();
  PTEST_END();

  PTEST_START("train/priority_queue");
  train_priority_queue_tests();
  PTEST_END();

  PTEST_START("train/path_finding");
  train_pathing_tests();
  PTEST_END();

  PRINTF("+----------------------+---+\n");
  PRINTF("|  ALL TESTS PASSED ☻  |   |\n");
  PRINTF("+----------------------+---+\n");

  return 0;
}

