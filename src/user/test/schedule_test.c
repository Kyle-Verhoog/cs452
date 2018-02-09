#include <schedule_test.h>

void TaskP0(){
	PRINTF("P0\n\r");
	Exit();
}
void TaskP1(){
	PRINTF("P1\n\r");
	Exit();
}
void TaskP2(){
	PRINTF("P2\n\r");
	Exit();
}
void TaskP3(){
	PRINTF("P3\n\r");
	Exit();
}
void TaskP4(){
	PRINTF("P4\n\r");
	Exit();
}
void TaskP5(){
	PRINTF("P5\n\r");
	Exit();
}
void TaskP6(){
	PRINTF("P6\n\r");
	Exit();
}
void TaskP7(){
	PRINTF("P7\n\r");
	Exit();
}
void TaskP8(){
	PRINTF("P8\n\r");
	Exit();
}
void TaskP9(){
	PRINTF("P9\n\r");
	Exit();
}

void TaskCompleter(){
	int ret;
	COMPLETE_TEST(ret);
}

void ScheduleCreator(){
	Create(0, &TaskP0);
	Create(1, &TaskP1);
	Create(2, &TaskP2);
	Create(3, &TaskP3);
	Create(4, &TaskP4);
	Create(5, &TaskP5);
	Create(6, &TaskP6);
	Create(7, &TaskP7);
	Create(8, &TaskP8);
	Create(9, &TaskP9);

	Create(0, &TaskCompleter);

	Exit();
}

void ScheduleTest(){
	Create(31, &ScheduleCreator);
	Exit();
}
