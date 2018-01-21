#include <kernel.h>

#define asm __asm__

// TODO move to main?
TaskDescriptor tasks[10];
task_queue tasks_queue;
int global_task_num;

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
  SANITY();
  DBLOG_INIT("Initializing", "");

  asm(
    "ldr r3, =KERNEL_ENTRY;"
    "mov r4, #"STR(KERNEL_ENTRY)";"
    "str r3, [r4];"
  );

  DBLOG_START("init task queue", "");
  tq_init(&tasks_queue);
  DBLOG_S();

  for (global_task_num = 0; global_task_num < 1; global_task_num++) {
    // td_init(tasks[global_task_num]);
    TaskDescriptor *td = &tasks[global_task_num];
    DBLOG_START("creating task %d", global_task_num);
    td_create(td, global_task_num, &taskOne, READY, NULL);
    DBLOG_S();
    DBLOG_START("pushing task %d to queue", global_task_num);
    tq_push(&tasks_queue, td);
    DBLOG_S();
  }
   // TaskDescriptor *td = &tasks[global_task_num];
   // td_create(td, global_task_num, &taskTwo, READY, NULL);
   // DBLOG_START("pushing task %d to queue", global_task_num);
   // tq_push(&tasks_queue, td);
   // global_task_num++;
   // DBLOG_S();
}

// Much TODO here
TaskDescriptor* schedule() {
  if (tasks_queue.size == 0)
    return NULL;

  int ret;
  TaskDescriptor *td = NULL;

  ret = tq_pop(&tasks_queue, &td);
  KASSERT(ret == 0 && td != NULL);


  return td;
}

TaskRequest activate(TaskDescriptor* td) {
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
  SWI_ARG_FETCH("r0");
}

void create(TaskDescriptor *td) {
  //Get the arguments r0 (priority) r1 (function pointer)
  int priority;
  void *task; 
  //read r0 and r1
  asm(
    "mov r3, %0;"::"r"(td->sp)
  );
  asm(
    "ldr r4, [r3, #4];"
    "ldr r5, [r3, #8];"
  );

  //Create the task
  asm(
    "mov %0, r4;":"=r"(priority):
  );
  asm(
    "mov %0, r5;":"=r"(task):
  );

  //TODO: FIX THIS ONCE SCHEDULING IS DONE
  if (global_task_num > 10) {
    asm(
      "str %0, [%1, #4]"::"r"(-2), "r"(td->sp)
    );
  }
  //else if(bad priority)
  else {
    TaskDescriptor *newTask = &tasks[global_task_num];
    td_create(newTask, global_task_num, task, READY, td);
    tq_push(&tasks_queue, newTask);
    global_task_num++;
    //set the tid of the newly created task back to the caller (in r0)
    asm(
      "str %0, [%1, #4]"::"r"(global_task_num - 1), "r"(td->sp)
    );  
  }
}

void handle(TaskDescriptor *td, TaskRequest req) {
  switch (req) {
    case PASS:
      KASSERT(false);
      tq_push(&tasks_queue, td);
      break;
    case BLOCK:
      td->status = BLOCKED;
      tq_push(&tasks_queue, td);
      break;
    case CREATE:
      create(td);
      tq_push(&tasks_queue, td);
      //SANITY();
      break;
    case MY_TID:
    case MY_PARENT_TID:
    case EXIT:
      td->status = ZOMBIE;
      break;
    default:
      KASSERT(false);
      tq_push(&tasks_queue, td);
      break;
  }
};

__attribute__((naked)) void main(void) {
  KERNEL_INIT();

  initialize();

  while (true) {
    //get a task from scheduler
    TaskDescriptor* td = schedule();

    if (!td) break;

    //activate task
    TaskRequest req = activate(td);

    //Handle the swi
    handle(td, req);
  }

  KERNEL_EXIT();
}
