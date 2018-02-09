#ifndef TEST_DEFINES_H
#define TEST_DEFINES_H

#define RUN_TEST_SYNC(req, ret, test) \
  PRINTF(STR(test)); \
	Create(20, &test); \
	Receive(&req, &ret, sizeof(ret)); \
	Reply(req, &ret, sizeof(ret)); \
	PRINTF(" \033[32m✓\033[0m\r\n");


#define COMPLETE_TEST() \
  int ret; \
  Send(0, &ret, sizeof(ret), &ret, sizeof(ret)); \
	Exit();
							

#endif //TEST_DEFINES_H
