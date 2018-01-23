#include <task.h>
#include <ts7200.h>
#include <bwio.h>

void tt_init(TidTracker *tt){
	init_circularBuffer(&tt->cb);
	int i = 0;
	for(i = 0; i < MAX_TASK; i++){
		tt->cb.buffer[tt->cb.buffer_end] = i;
		tt->cb.buffer_end = (tt->cb.buffer_end + 1) % CIRCULAR_BUFFER_SIZE;
	}
}

int tt_get(TidTracker *tt){
	if(tt->cb.buffer_end == tt->cb.buffer_start){
		return -2;
	}
	int tid = tt->cb.buffer[tt->cb.buffer_start];
	tt->cb.buffer_start = (tt->cb.buffer_start + 1) % CIRCULAR_BUFFER_SIZE;
	return tid;
}

void tt_return(int tid, TidTracker *tt){
	tid += (1 << 16);
	tt->cb.buffer[tt->cb.buffer_end] = tid;
	tt->cb.buffer_end = (tt->cb.buffer_end + 1) % CIRCULAR_BUFFER_SIZE;
}


void td_init(TaskDescriptor *td) {
  td->tid = 0;
  td->sp =  0;
  td->psr = 0;
  td->task = NULL;
  td->status = UNINIT;
  td->stack_base = 0;
  td->parent = NULL;
  td->priority = 1;
}

void td_create(TaskDescriptor *td, uint32_t tid, void *task, int priority, TaskStatus status, TaskDescriptor *parent) {
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
  td->priority = priority;
  td->next = NULL;
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
	int tid = MyTid();

	bwprintf(COM2, "T1 - %x: START\r\n", tid);
	Pass();

	bwprintf(COM2, "T1 - %x: MADE NEW TASK: %x\n\r", tid, Create(5, &taskTwo));
	Pass();

	bwprintf(COM2, "T1 - %x: EXIT\n\r", tid);
  // EXIT
  Exit();
}


void taskTwo() {
	int tid = MyTid();
  bwprintf(COM2, "T2 - %x: START\r\n", tid);

	bwprintf(COM2, "T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));
	bwprintf(COM2, "T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));
	bwprintf(COM2, "T2 - %x: MADE NEW TASK: %x\n\r",tid, Create(5, &taskTwo));

  bwprintf(COM2, "T2 - %x: EXIT\r\n", tid);
  Exit();
}

/**
 * K1 Assignment Test Tasks
**/

void OtherTask(){
	int tid = MyTid();
	int parentTid = MyParentTid();

	bwprintf(COM2, "Tid: %d, parentTid: %d\n\r", tid, parentTid);
	Pass();
	bwprintf(COM2, "Tid: %d, parentTid: %d\n\r", tid, parentTid);

	Exit();
}

void FirstUserTask(){
	int val = Create(0, &OtherTask);
	bwprintf(COM2, "Created: %d\n\r", val);
	val = Create(0, &OtherTask);
	bwprintf(COM2, "Created: %d\n\r", val);
	val = Create(5, &OtherTask);
	bwprintf(COM2, "Created: %d\n\r", val);
	val = Create(5, &OtherTask);
	bwprintf(COM2, "Created: %d\n\r", val);

	bwprintf(COM2, "FirstUserTask: exiting.\n\r");
	Exit();
}
