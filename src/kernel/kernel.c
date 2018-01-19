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
	"mov r4, sp;"                 \
	"mov sp, r1;"                 \
	"stmfd sp!, {r4, lr};"        \
);

/**
 * Restores the old SP in order to return to RedBoot.
 */
#define KERNEL_EXIT() asm( \
  ".extern kernel_stack_base;"  \
	"ldr r1, =kernel_stack_base;" \
	"ldr r1, [r1];"               \
  "sub r1, r1, #72;"            \
	"ldmfd r1, {sp, pc};"         \
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

// Much TODO here
TaskDescriptor* schedule() {
  int ret;
  TaskDescriptor *td = NULL;
  do {
    ret = tq_pop(&tasks_queue, &td);
    KASSERT(ret == 0 && td != NULL);
  } while (!td->status == READY && tasks_queue.size > 0);

  if (tasks_queue.size == 0)
    return NULL;

  return td;
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

  // manually put swi arg in r0, avoid overhead of return
  asm(
    "ldr r0, [lr, #-4];"
    "bic r0, r0, #0xff000000;"
  );
}

void handle(TaskDescriptor *td, KernelRequest req) {
  switch (req) {
    case EXIT:
      td->status = ZOMBIE;
      break;
    default:
      tq_push(&tasks_queue, td);
      break;
  }
};

__attribute__((naked)) void main(void) {
  KERNEL_INIT();
  PRINT_REG("lr");
  PRINT_REG("r4");

  initialize();

  while (true) {
    //get a task from scheduler
    TaskDescriptor* td = schedule();

    if (!td) break;

    //activate task
    KernelRequest req = activate(td);

    //Handle the swi
    handle(td, req);
  }

  KERNEL_EXIT();
}

