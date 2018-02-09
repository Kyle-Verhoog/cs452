#ifndef TEST_DEFINES_H
#define TEST_DEFINES_H

#include <messaging.h>

#define RUN_TEST_SYNC(req, ret, test) PRINTF("STARTING "STR(test)"\n\r"); \
								Create(20, &test); \
								Receive(&req, &ret, sizeof(ret)); \
	  							Reply(req, &ret, sizeof(ret)); \
	  							PRINTF("ENDING "STR(test)"\n\r");


#define COMPLETE_TEST(ret) 	Send(0, &ret, sizeof(ret), &ret, sizeof(ret)); \
							Exit();
							

#endif //TEST_DEFINES_H
