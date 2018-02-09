#include <taskid_test.h>

void PrintMyTaskId(){
	tid_t tid = MyTid();
	PRINTF("%x\n\r", tid);

	Exit();
}

void TaskIdTest(){
	int i, j;
	for(i = 0; i < 100; i++){
		for(j = 0; j < 1; j++){
			Create(31, &PrintMyTaskId);
		}
	}

	int ret;
	COMPLETE_TEST(ret);
}
