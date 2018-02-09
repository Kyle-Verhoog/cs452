#include <kernel/kernel.h>

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
  DBLOG_INIT("Initializing", "");

  init_irq(&im_tasks);

  int i;
  for (i = 0; i < MAX_TASK; i++) {
    td_init(&tasks[i]);
  }

  DBLOG_START("init task queue", "");
  tt_init(&tid_tracker);
  pq_init(&pq_tasks);
  DBLOG_S();

  DBLOG_START("init interrupt matrix", "");
  im_init(&im_tasks);
  DBLOG_S();

  int priority;
  void *task;

#ifdef KTEST
  priority = 0;
  task = &TestTask;
#else
  priority = 0;
  #ifdef METRIC_64
    task = &K2InitMetricTask;
  #elif METRIC_4
    task = &K2InitMetricTask;
  #else
    task = &K3FirstUserTask;
  #endif
#endif

  tid_t tid = tt_get(&tid_tracker);
  TaskDescriptor* volatile td = &tasks[(tid & 0xffff)];

  DBLOG_START("creating task %x", tid);
  ktd_create(td, tid, task, priority, READY, NULL);
  DBLOG_S();
  DBLOG_START("pushing task %x to queue", tid);
  pq_push(&pq_tasks, priority, td);
  DBLOG_S();
}

// Much TODO here
TaskDescriptor* schedule() {
  if (pq_tasks.size == 0)
    return NULL;

  int ret;
  TaskDescriptor *td = NULL;

  //PRINTF("Tasks Left:%d\n\r", MAX_TASK - tid_tracker.cb.size);

  ret = pq_pop(&pq_tasks, &td);
  KASSERT(ret == 0 && td != NULL);

  return td;
}

TaskRequest activate(TaskDescriptor* td) {
  if(td->it) {
    PUSH_STACK("r0-r12, lr");
    WRITE_SPSR(td->psr);
    
    SET_CPSR(SYSTEM_MODE);
    WRITE_SP(td->sp);
    POP_STACK("lr");
    asm("mov r8, sp");
    asm("add sp, sp, #56");

    SET_CPSR(KERNEL_MODE);
    asm("ldmfd r8, {r0-r12, lr}");
    asm("SUBS pc, lr, #4");
    //REVERSE_SWI();
  }else{
    PUSH_STACK("r0-r12, lr"); // TODO: kernel lr
    asm("mov r8, %0"::"r"(td->ret));
    PUSH_STACK("r8");
    WRITE_SPSR(td->psr);
    
    SET_CPSR(SYSTEM_MODE);
    WRITE_SP(td->sp);
    POP_STACK("lr");
    asm("mov r8, sp");
    asm("add sp, sp, #56");

    SET_CPSR(KERNEL_MODE);
    asm("ldmfd r8, {r0-r12, lr}");
    POP_STACK("r0");
    REVERSE_SWI();  
  }

  //=============================================================//

  asm("IRQ_ENTRY:");
  asm("stmfd sp, {r0-r12};");

  asm("mov r9, sp;"
      "mov r10, lr;");

  //Change to System
  SET_CPSR(SYSTEM_MODE);

  asm("ldmdb r9!, {r0-r7}");
  asm("stmdb sp!, {r0-r7, r10}");
  asm("ldmdb r9!, {r0-r4}");
  asm("stmdb sp!, {r0-r4}");
  PUSH_STACK("lr");

  SET_CPSR(KERNEL_MODE);
  POP_STACK("r0-r12");
  SET_CPSR(SYSTEM_MODE);
  READ_SP(td->sp);
  SET_CPSR(IRQ_MODE);
  READ_SPSR(td->psr);
  SET_CPSR(KERNEL_MODE);
  td->it = 1;

  asm("mov r0, #12"); //ENUM - TR_IRQ
  POP_STACK("lr");
  asm("b ACTIVATE_END");

  //=============================================================//

  //AFTER USER TASK CALLS SWI (CANT USE FP)
  asm("KERNEL_ENTRY:");
  asm("stmfd sp, {r0-r12};");

  asm("mov r9, sp;"
      "mov r10, lr;");

  //Change to System
  SET_CPSR(SYSTEM_MODE);

  asm("ldmdb r9!, {r0-r7}");
  asm("stmdb sp!, {r0-r7, r10}");
  asm("ldmdb r9!, {r0-r4}");
  asm("stmdb sp!, {r0-r4}");
  PUSH_STACK("lr");

  SET_CPSR(KERNEL_MODE);
  POP_STACK("r0-r12");
  SET_CPSR(SYSTEM_MODE);
  READ_SP(td->sp);
  SET_CPSR(KERNEL_MODE);
  READ_SPSR(td->psr);
  td->it = 0;
  SWI_ARG_FETCH("r0");
  POP_STACK("lr");

  //=============================================================//

  asm("ACTIVATE_END:");
}

void create(TaskDescriptor *td) {
  //Get the arguments r0 (priority) r1 (function pointer)
  tid_t tid = tt_get(&tid_tracker);
  int priority;
  void *task;

  asm("ldr %0, [%1, #4];":"=r"(priority):"r"(td->sp));
  asm("ldr %0, [%1, #8];":"=r"(task):"r"(td->sp));

  //TODO: FIX THIS ONCE SCHEDULING IS DONE
  if (tid < 0) {
    td->ret = -2;
    KASSERT(false && "Out of Tids");
  }
  //else if(bad priority)
  else {
    TaskDescriptor *newTask = &tasks[TID_ID(tid)];
    ktd_create(newTask, tid, task, priority, READY, td);
    pq_push(&pq_tasks, priority, newTask);
    td->ret = tid;
  }
}

void get_tid(TaskDescriptor *td) {
  td->ret = td->tid;
}

void get_parentTid( TaskDescriptor *td) {
  //Get the parent tid into user stack
  // asm(
  //   "str %0, [%1, #4]"::"r"(td->parent ? td->parent->tid : -1), "r"(td->sp)
  // );
  td->ret = td->parent ? td->parent->tid : -1;
}

void handle(TaskDescriptor *td, TaskRequest req) {
  switch (req) {
  case TR_ASSERT:
    KABORT();
    break;
  case TR_PASS:
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_BLOCK:
    td->status = BLOCKED;
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_CREATE:
    create(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_MY_TID:
    get_tid(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_MY_PARENT_TID:
    get_parentTid(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_SEND:
    send_handler(td);
    break;
  case TR_RECEIVE:
    receive_handler(td);
    break;
  case TR_REPLY:
    reply_handler(td);
    break;
  case TR_NS_REG:
    ns_register(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_NS_GET:
    ns_get(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_EXIT:
    // TODO: uninitialize the task descriptor
    td->status = ZOMBIE;
    tt_return(td->tid, &tid_tracker);
    break;
  case TR_IRQ:
    event_wake(&im_tasks);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_AWAIT_EVENT:
    event_register(&im_tasks, td);
    break;
  default:
    KASSERT(false && "UNDEFINED SWI PARAM");
    pq_push(&pq_tasks, td->priority, td);
    break;
  }
};

int no_tasks() {
  return tid_tracker.cb.size == MAX_TASK;
}


// TODO: fix this
// NOTE: sl register not loaded
//       YOU CANNOT USE GLOBALS IN MAIN
__attribute__((naked)) int main(void) {
  KERNEL_INIT();
#ifdef CACHE
  ENABLE_ALL_CACHE();
#endif

  asm(
    "ldr r3, =KERNEL_ENTRY;"
    "mov r4, #"STR(SWI_ENTRY)";"
    "str r3, [r4];"
  );
  asm(
    "ldr r3, =IRQ_ENTRY;"
    "mov r4, #"STR(IRQ_ENTRY)";"
    "str r3, [r4];"
  );

  initialize();

  while (true) {
    //get a task from scheduler
    TaskDescriptor* td = schedule();

    if (!td && no_tasks()) break;

    if (!td) continue;

    //activate task
    TaskRequest req = activate(td);

    //Handle the swi
    handle(td, req);
  }

  cleanup_irq();
#ifdef CACHE
  DISABLE_ALL_CACHE();
#endif

  KERNEL_EXIT();
  return 0; // should not be reachable!
}


