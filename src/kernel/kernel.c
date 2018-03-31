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
  interrupt_init();

  int i;
  for (i = 0; i < MAX_TASK; i++) {
    td_init(&tasks[i]);
  }

  tt_init(&tid_tracker);
  pq_init(&pq_tasks);
  im_init(&im_tasks);

  tm_init();

  int priority;
  void *task;

#ifdef KTEST
  priority = 0;
  task = &TestTask;
#else
  priority = 0;
  task = &Bootstrap;
  // priority = 20;
  // task = &IOServerTest;
#endif //KTEST

  tid_t tid = tt_get(&tid_tracker);
  TaskDescriptor* volatile td = &tasks[TID_ID(tid)];

  ktd_create(td, tid, task, priority, TS_READY, NULL);
  pq_push(&pq_tasks, priority, td);
}

TaskDescriptor* schedule() {
  if (pq_tasks.size == 0)
    return NULL;

  int ret;
  TaskDescriptor *td = NULL;

  ret = pq_pop(&pq_tasks, &td);
  KASSERT(ret == 0 && td != NULL);

  return td;
}

TaskRequest activate(TaskDescriptor* td) {
  if (td->it) {
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
  } else {
    PUSH_STACK("r0-r12, lr");
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

void get_tid(TaskDescriptor *td) {
  td->ret = td->tid;
}

void get_parentTid( TaskDescriptor *td) {
  td->ret = td->parent ? td->parent->tid : -1;
}

int calc_cpu_time(TaskDescriptor *td) {
  int id;
  tid_t other_tid;
  TaskDescriptor *other;

  asm("ldr %0, [%1, #4];":"=r"(other_tid):"r"(td->sp));

  id = TID_ID(other_tid);
  other = &tasks[id];
  assert(*(int*)(TM_CLOCK_VAL) < other->start_time);
  return (other->running_time * 100) / (other->start_time - *(int*)(TM_CLOCK_VAL));
}

int task_info(TaskDescriptor *td) {
  int i, offset, status;
  char *buf;
  const char *stat[] = {
    TEXT_GREEN"ACTIV"TEXT_BLACK,
    TEXT_GREEN"READY"TEXT_BLACK,
    TEXT_RED"REGBL"TEXT_BLACK,
    TEXT_RED"UNINI"TEXT_BLACK,
    TEXT_RED"ZOMBI"TEXT_BLACK,
    TEXT_RED"RCVBL"TEXT_BLACK,
    TEXT_RED"SNDBL"TEXT_BLACK,
    TEXT_RED"RPLBL"TEXT_BLACK,
    TEXT_YELLOW"RUNNG"TEXT_BLACK,
  };
  asm("ldr %0, [%1, #4];":"=r"(buf):"r"(td->sp));
  offset = 0;

  offset += buf_pack_str(buf+offset, "\vtid\tstatus\n");
    offset += buf_pack_f(buf+offset, "┉┉┉┉┉┉┉┉┉┉┉┉┉\n");
  for (i = 0; i < MAX_TASK; ++i) {
    status = tasks[i].status;
    if (status != TS_ZOMBIE && status != TS_UNINIT)
      offset += buf_pack_f(buf+offset, "%d %d\t%s\n", i, tasks[i].priority, stat[status]);
  }
  return offset;
}

void handle(TaskDescriptor *td, TaskRequest req) {
  switch (req) {
  case TR_IRQ:
    event_wake(&im_tasks);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_AWAIT_EVENT:
    event_register(&im_tasks, td);
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
  case TR_PASS:
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_BLOCK:
    td->status = TS_BLOCKED;
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_CREATE:
    create(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_CREATE_W_ARGS:
    create_w_args(td);
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
  case TR_NS_REG:
    ns_register(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_NS_GET:
    ns_get(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_EXIT:
#ifdef TASK_METRICS
    tm_addSummary(td);
#endif //TASK_METRICS
    td->status = TS_ZOMBIE;
    tt_return(td->tid, &tid_tracker);
    break;
  case TR_ASSERT:
#ifdef TASK_METRICS
    tm_summarize();
#endif //TASK_METRICS
    KABORT();
    break;
  case TR_HALT:
    KEXIT();
    break;
  case TR_INFO_NPROC:
    td->ret = MAX_TASK - tid_tracker.cb.size;
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_INFO_MEM:
    td->ret = calc_mem_usage();
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_INFO_CPU:
    td->ret = calc_cpu_time(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case TR_INFO_TASK:
    td->ret = task_info(td);
    pq_push(&pq_tasks, td->priority, td);
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
    int st, et; //start & end time

    //get a task from scheduler
    TaskDescriptor* td = schedule();
    td->status = TS_RUNNING;

    if (!td && no_tasks()) break;
    if (!td) continue;

    st = *(int *)TM_CLOCK_VAL;

    //activate task
    TaskRequest req = activate(td);
//    td->status = TS_READY;

    et = *(int *)TM_CLOCK_VAL;
    tm_delta(st, et, td);

    //Handle the swi
    handle(td, req);
  }

#ifdef TASK_METRICS
  tm_summarize();
#endif //TASK_METRICS

  interrupt_cleanup();

#ifdef CACHE
  DISABLE_ALL_CACHE();
#endif //CACHE

  KERNEL_EXIT();
  return 0; // should not be reachable!
}


