#include <taskid_test.h>

void PrintMyTaskId(){
	int tid = MyTid();
	PRINTF("%x\n\r", tid);

	Exit();
}

void TaskIdTest(){
	int i;
	for(i = 0; i < 100; i++){
		Create(31, &PrintMyTaskId);
	}

	int ret;
	COMPLETE_TEST(ret);
}