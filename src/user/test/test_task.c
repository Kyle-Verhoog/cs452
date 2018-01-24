#include <user/test/test_task.h>


void Stall(){
	int stallTime = 100000;
	int i;
	for(i = 0; i < stallTime; i++){
		//Stalling
		Pass();
	}
}

void TestRegisters(){
	int tid = MyTid();

}

void TestTask(){
	bwprintf(COM2, "Starting firstUserTask:\n\r");
	Create(5, &FirstUserTask);
	bwprintf(COM2, "All tests completed.\n\r");
	Exit();
}