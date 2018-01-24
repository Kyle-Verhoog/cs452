#include <kernel.h>

#define asm __asm__

// TODO move to main?
//uint32_t active_task.psr_temp; //Used as a active_task.psr_temp to set CPSR
TaskDescriptor tasks[MAX_TASK];
TidTracker tid_tracker;
priority_queue pq_tasks;
int global_task_num;


/**
 * Saves the old SP to the new kernel stack and sets SP to the kernel stack.
 */
#define KERNEL_INIT() \
  asm( \
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
#define KERNEL_EXIT() \
  asm( \
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
  tt_init(&tid_tracker);
  pq_init(&pq_tasks);
  DBLOG_S();

  int i;
  for (i = 0; i < 1; i++) {
    int priority = 2;
    // td_init(tasks[global_task_num]);
    int tid = tt_get(&tid_tracker);
    TaskDescriptor *td = &tasks[(tid & 0xffff)];
    DBLOG_START("creating task %x", tid);
#ifdef  KTEST
    ktd_create(td, tid, &TestTask, 0, READY, NULL);
#else
    ktd_create(td, tid, &FirstUserTask, priority, READY, NULL);
#endif
    DBLOG_S();
    DBLOG_START("pushing task %x to queue", tid);
    pq_push(&pq_tasks, priority, td);
    DBLOG_S();
  }
}

// Much TODO here
TaskDescriptor* schedule() {
  if (pq_tasks.size == 0)
    return NULL;

  int ret;
  TaskDescriptor *td = NULL;

  ret = pq_dumb_pop(&pq_tasks, &td);
  KASSERT(ret == 0 && td != NULL);

  return td;
}

TaskRequest activate(TaskDescriptor* td) {
  //Store Kernel State
  PUSH_STACK("r0-r12");
  //Push ret val to stack as temp
  asm("mov r8, %0"::"r"(td->ret));
  PUSH_STACK("r8");
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
  //Set r0 with the new return value from stack
  POP_STACK("r0");
  //Move to the user task
  REVERSE_SWI();

  //AFTER USER TASK CALLS SWI (CANT USE FP)
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
  int tid = tt_get(&tid_tracker);
  int priority;
  void *task;

  asm("ldr %0, [%1, #4];":"=r"(priority):"r"(td->sp));
  asm("ldr %0, [%1, #8];":"=r"(task):"r"(td->sp));

  //TODO: FIX THIS ONCE SCHEDULING IS DONE
  if (tid < 0 /*|| (tid & 0xffff) >= 10*/) {
    td->ret = -2;
    KASSERT(false && "Out of Tids");
  }
  //else if(bad priority)
  else {
    TaskDescriptor *newTask = &tasks[(tid & 0xffff)];
    ktd_create(newTask, tid, task, priority, READY, td);
    pq_push(&pq_tasks, priority, newTask);
    td->ret = tid;
  }
}

void get_tid(TaskDescriptor *td) {
  td->ret = td->tid;
}

void get_parentTid(TaskDescriptor *td) {
  //Get the parent tid into user stack
  // asm(
  //   "str %0, [%1, #4]"::"r"(td->parent ? td->parent->tid : -1), "r"(td->sp)
  // );
  td->ret = td->parent ? td->parent->tid : -1;
}

void handle(TaskDescriptor *td, TaskRequest req) {
  switch (req) {
  case ASSERT:
    KABORT();
    break;
  case PASS:
    pq_push(&pq_tasks, td->priority, td);
    break;
  case BLOCK:
    td->status = BLOCKED;
    pq_push(&pq_tasks, td->priority, td);
    break;
  case CREATE:
    create(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case MY_TID:
    get_tid(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case MY_PARENT_TID:
    get_parentTid(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case EXIT:
    td->status = ZOMBIE;
    tt_return(td->tid, &tid_tracker);
    break;
  default:
    KASSERT(false);
    pq_push(&pq_tasks, td->priority, td);
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
