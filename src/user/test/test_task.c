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
	int reg;
	int r9, r10, r11, r12;
	int tid = MyTid();
	bwprintf(COM2, "TestRegisters - %x: \n\r", tid);

	asm("mov r0, #0");
	asm("mov r1, #1");
	asm("mov r2, #2");
	asm("mov r3, #3");
	asm("mov r4, #4");
	asm("mov r5, #5");
	asm("mov r6, #6");
	asm("mov r7, #7");
	asm("mov r8, #8");
	asm("mov %0, r9":"=r"(r9):);
	asm("mov %0, r10":"=r"(r10):);
	asm("mov %0, r11":"=r"(r11):);
	asm("mov %0, r12":"=r"(r12):);
	// asm("mov r9, #9");
	// asm("mov r10, #10");
	// asm("mov r11, #11");
	// asm("mov r12, #12");

	Pass();

	asm("mov %0, r4":"=r"(reg):);
	assert(reg == 4);
	asm("mov %0, r5":"=r"(reg):);
	assert(reg == 5);
	asm("mov %0, r6":"=r"(reg):);
	assert(reg == 6);
	asm("mov %0, r7":"=r"(reg):);
	assert(reg == 7);
	asm("mov %0, r8":"=r"(reg):);
	assert(reg == 8);
	asm("mov %0, r9":"=r"(reg):);
	assert(reg == r9);
	asm("mov %0, r10":"=r"(reg):);
	assert(reg == r10);
	asm("mov %0, r11":"=r"(reg):);
	assert(reg == r11);
	// asm("mov %0, r12":"=r"(reg):);
	// assert(reg == r12);

	Exit();
}

void TestTask(){
	bwprintf(COM2, "Starting firstUserTask:\n\r");
	Create(5, &FirstUserTask);
	Create(5, &TestRegisters);
	bwprintf(COM2, "All tests completed.\n\r");
	Exit();
}
