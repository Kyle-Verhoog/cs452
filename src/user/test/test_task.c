#include <user/test/test_task.h>

void TestTask() {
  tid_t req;
  int ret;
  // RUN_TEST_SYNC(req, ret, ContextSwitchTest);
  // RUN_TEST_SYNC(req, ret, MessagingTest);
  // RUN_TEST_SYNC(req, ret, MessagingTest);
  // RUN_TEST_SYNC(req, ret, TaskIdTest);
  // RUN_TEST_SYNC(req, ret, TaskIdTest);
  // RUN_TEST_SYNC(req, ret, NameServerTest);
  // RUN_TEST_SYNC(req, ret, NameServerTest);
  // RUN_TEST_SYNC(req, ret, ScheduleTest);
  // RUN_TEST_SYNC(req, ret, ScheduleTest);
  // RUN_TEST_SYNC(req, ret, ClockServerTest);
  // RUN_TEST_SYNC(req, ret, ClockServerTest);
  // RUN_TEST_SYNC(req, ret, CreateArgsTest);
  RUN_TEST_SYNC(req, ret, UITest);
  //Create(20, &IOServerTest);
  // RUN_TEST_SYNC(req, ret, IOServerTest);
  Exit();
}
