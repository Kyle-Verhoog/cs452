#include <user/test/test_task.h>


void Stall(){
	int stallTime = 1000;
	int i;
	for(i = 0; i < stallTime; i++){
		//Stalling
		bwprintf(COM2, "%d \n\r", i);
		Pass();
	}

	Exit();
}

void TestRegisters(){
	//volatile int tid = MyTid();
}

void TestTask(){
	bwprintf(COM2, "Starting firstUserTask:\n\r");
	Create(5, &FirstUserTask);
	//Create(1, &Stall);
	bwprintf(COM2, "All tests completed.\n\r");
	Exit();
}
