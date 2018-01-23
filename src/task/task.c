#include <task.h>
#include <ts7200.h>
#include <bwio.h>

void td_init(TaskDescriptor *td) {
  td->tid = 0;
  td->sp =  0;
  td->psr = 0;
  td->task = NULL;
  td->status = UNINIT;
  td->stack_base = 0;
  td->parent = NULL;
}

void td_create(TaskDescriptor *td, uint32_t tid, void *task, TaskStatus status, TaskDescriptor *parent) {
  //Initialize the Test task pc
  td->sp = USER_STACK_BASE - (tid*USER_STACK_SIZE) - 56;
  asm("mov r8, %0;"::"r"(task));
  asm("stmfd %0!, {r8};"::"r"(td->sp));
  td->sp -= 4; //saved lr_svc
  // td->sp = USER_STACK_BASE - (tid*USER_STACK_SIZE) - 56;
  // SET_CPSR(SYSTEM_MODE);
  // WRITE_SP(td->sp);
  // asm("mov r8, %0;"::"r"(task));
  // PUSH_STACK("r8");
  // SET_CPSR(KERNEL_MODE);
  // td->sp -= 4; //saved lr_svc

  td->tid = tid;
  td->psr = USER_MODE;
  td->task = task;
  td->status = status;
  td->parent = parent;
  td->ret = 0;
}

/**
 * Copies the values of td2 into td1
 */
void td_copy(TaskDescriptor *td1, TaskDescriptor *td2) {
  td1->tid = td2->tid;
  td1->sp = td2->sp;
  td1->psr = td2->psr;
  td1->task = td2->task;
  td1->status = td2->status;
  td1->stack_base = td2->stack_base;
}

/**
 * SysCalls (TODO: PASS ENUM LITERAL)
 */
void Pass(){
	asm (
		"swi #0;"
	);
}

void Block(){
	asm (
		"swi #1;"
	);
}

int Create(int priority, void (*code)()){
	//r0 = priority, r1 = code
	//Call swi with create

	asm(
		"swi #2;"
	);

	//Expect from here r0 is set already as the return value
}

int MyTid(){
	//Call swi with myTid
	asm(
		"swi #3;"
	);

	//Expect r0 set
}

int MyParentTid(){
	//Call swi with myTid
	asm(
		"swi #4;"
	);

	//Expect r0 set	
}

void Exit(){
	asm (
		"swi #5;"
	);
}

void taskOne() {
	bwprintf(COM2, "T1: START\r\n");
	Pass();

	bwprintf(COM2, "T1: CREATE\n\r");

	bwprintf(COM2, "T1: MADE NEW TASK: %d\n\r", Create(5, &taskTwo));

	bwprintf(COM2, "T1: THIS IS MY TID: %d\n\r", MyTid());
	bwprintf(COM2, "T1: THIS IS MY PARENT'S TID: %d\n\r", MyParentTid());

	bwprintf(COM2, "T1 EXIT\n\r");
  // EXIT
  Exit();
}


void taskTwo() {
  bwprintf(COM2, "T2 START\r\n");
  //Block();
  Pass();

	bwprintf(COM2, "T2: THIS IS MY TID: %d\n\r", MyTid());
	bwprintf(COM2, "T2: THIS IS MY PARENT'S TID: %d\n\r", MyParentTid());

	bwprintf(COM2, "T2: MADE NEW TASK: %d\n\r", Create(5, &taskOne));

  bwprintf(COM2, "T2 EXIT\r\n");
  Exit();
}
