#include <kernel.h>

#define asm __asm__

// TODO move to main?
TaskDescriptor tasks[10];
task_queue tasks_queue;

/**
 * Saves the old SP to the new kernel stack and sets SP to the kernel stack.
 */
#define KERNEL_INIT() asm(      \
  ".extern kernel_stack_base;"  \
	"ldr r1, =kernel_stack_base;" \
	"ldr r1, [r1];"               \
  "sub fp, r1, #4;"             \
  "sub r1, r1, #64;"            \
	"mov r2, sp;"                 \
	"mov sp, r1;"                 \
	"stmfd sp!, {r2};"            \
);

/**
 * Restores the old SP in order to return to RedBoot.
 */
#define KERNEL_EXIT() asm( \
  "KERNEL_EXIT:"           \
	"ldmfd sp, {sp};"        \
);


unsigned int kernel_stack_base = KERNEL_STACK_BASE;
unsigned int user_stack_base = USER_STACK_BASE;

void initialize() {
  DBLOG_INIT("Initializing", "");

  asm(
    "ldr r3, =KERNEL_ENTRY;"
    "mov r4, #"STR(KERNEL_ENTRY)";"
    "str r3, [r4];"
  );

  DBLOG_START("init task queue", "");
  tq_init(&tasks_queue);
  DBLOG_S();

  int i;
  for (i = 0; i < 8; i++) {
    // td_init(tasks[i]);
    TaskDescriptor *td = &tasks[i];
    DBLOG_START("creating task %d", i);
    td_create(td, i, &taskOne, READY);
    DBLOG_S();
    DBLOG_START("pushing task %d to queue", i);
    tq_push(&tasks_queue, td);
    DBLOG_S();
  }
}

TaskDescriptor* schedule() {
  int ret;
  TaskDescriptor *t = NULL;
  ret = tq_pop(&tasks_queue, &t);
  tq_push(&tasks_queue, t);
  KASSERT(ret == 0 && t != NULL);
  return t;
}

KernelRequest activate(TaskDescriptor* td) {
  //Store Kernel State
  PUSH_STACK("r0-r12");
  //Install SPSR
  WRITE_SPSR(td->psr);
  //Change to system mode
  SET_CPSR(SYSTEM_MODE);
  //Change the stack pointer to the task's stack (uses fp so no worries)
  WRITE_SP(td->sp);
  //Load instruction after swi (r4) from user stack
  POP_STACK("r4");
  //Change to kernel mode
  SET_CPSR(KERNEL_MODE);
  //Save into kernel lr for loading
  asm("mov lr, r4;");
  //Change to system mode
  SET_CPSR(SYSTEM_MODE);
  //Load the User Trap Frame
  POP_STACK("r0-r12, lr");
  //Switch back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //Move to the user task
  REVERSE_SWI();

  //AFTER USER TASK CALLS SWI
  asm("KERNEL_ENTRY:");

  //Change to System mode
  SET_CPSR(SYSTEM_MODE);
  //Save the user state
  PUSH_STACK("r0-r12, lr");
  //Change to Kernel mode
  SET_CPSR(KERNEL_MODE);
  //Save lr to stratch r3
  asm("mov r3, lr");
  //Change to System mode
  SET_CPSR(SYSTEM_MODE);
  //Save the lr(r3)
  PUSH_STACK("r3")
  //Change back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //load the kernel stack (fp is now resuable again!)
  POP_STACK("r0-r12");
  //Change back to system mode
  SET_CPSR(SYSTEM_MODE); //Note we can still use fp!
  //Save the user sp to TaskDescriptor's sp
  READ_SP(td->sp);
  //Change back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //Save the spsr to the TaskDescriptor's psr
  READ_SPSR(td->psr);

  //Return SWI Argument
  return PASS;
}

void handle(KernelRequest req) {
  //Switch Statement
};

int main(void) {
  KERNEL_INIT();

  initialize();

  while(true) {
    //get a task from scheduler
    TaskDescriptor* td = schedule();

    KASSERT(td != NULL);

    //activate task
    KernelRequest req = activate(td);

    //Handle the swi
    handle(req);
  }

  KERNEL_EXIT();
	return 0;
}

