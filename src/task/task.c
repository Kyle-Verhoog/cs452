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
}

void td_create(TaskDescriptor *td, uint32_t tid, uint32_t sp, uint32_t psr, void *task, TaskStatus status) {
  //Initialize the Test task pc
  SET_CPSR(SYSTEM_MODE);
  WRITE_SP(USER_STACK_BASE - (tid*USER_STACK_SIZE) - 56);
  asm("mov r3, %0;"::"r"(task));
  PUSH_STACK("r3");
  SET_CPSR(KERNEL_MODE);
  sp -= 4; //saved lr_svc

  td->tid = tid;
  td->sp = sp;
  td->psr = psr;
  td->task = task;
  td->status = status;
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

void taskOne(){
	int counter = 0;
	bwprintf(COM2, "PRINT %d\n\r", counter);
	counter++;

	asm(
		"swi #0;"
	);
	
	bwprintf(COM2, "PRINT %d\n\r", counter);
	counter++;

	asm(
		"swi #0;"
	);

	bwprintf(COM2, "PRINT %d\n\r", counter);
	bwprintf(COM2, "PRINT %d AGAIN\n\r", counter);
	counter++;

	asm(
		"swi #0;"
	);	
}
