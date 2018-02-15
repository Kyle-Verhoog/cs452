#include <schedule_test.h>

void TaskP0(){
	//PRINTF("P0\n\r");
	Exit();
}
void TaskP1(){
	//PRINTF("P1\n\r");
	Exit();
}
void TaskP2(){
	//PRINTF("P2\n\r");
	Exit();
}
void TaskP3(){
	//PRINTF("P3\n\r");
	Exit();
}
void TaskP4(){
	//PRINTF("P4\n\r");
	Exit();
}
void TaskP5(){
	//PRINTF("P5\n\r");
	Exit();
}
void TaskP6(){
	//PRINTF("P6\n\r");
	Exit();
}
void TaskP7(){
	//PRINTF("P7\n\r");
	Exit();
}
void TaskP8(){
	//PRINTF("P8\n\r");
	Exit();
}
void TaskP9(){
	//PRINTF("P9\n\r");
	Exit();
}

void TaskCompleter(){
	COMPLETE_TEST();
}

void ScheduleCreator(){
	Create(22, &TaskP0);
	Create(23, &TaskP1);
	Create(24, &TaskP2);
	Create(25, &TaskP3);
	Create(26, &TaskP4);
	Create(27, &TaskP5);
	Create(28, &TaskP6);
	Create(29, &TaskP7);
	Create(30, &TaskP8);
	Create(31, &TaskP9);

	Create(22, &TaskCompleter);

	Exit();
}

void ScheduleTest(){
	Create(31, &ScheduleCreator);
	Exit();
}
