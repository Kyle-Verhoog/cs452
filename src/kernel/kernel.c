#include <kernel.h>

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

void init_irq(){
  SET_CPSR(IRQ_MODE);
  WRITE_SP(IRQ_STACK_BASE);
  SET_CPSR(KERNEL_MODE);

  *(int *)(VIC1_BASE + VIC_PROTECTION_OFFSET) = 0;
  *(int *)(VIC2_BASE + VIC_PROTECTION_OFFSET) = 0;
  //Enable Hardware Interrupts
  //*(int *)(VIC1_BASE + VIC_INTENABLE_OFFSET) = 3;
  //*(int *)(VIC2_BASE + VIC_INTENABLE_OFFSET) = 3;
}

void initialize() {
  SANITY();
  DBLOG_INIT("Initializing", "");

  init_irq();

  int i;
  for (i = 0; i < MAX_TASK; i++) {
    td_init(&tasks[i]);
  }

  DBLOG_START("init task queue", "");
  tt_init(&tid_tracker);
  pq_init(&pq_tasks);
  DBLOG_S();

  int priority;
  void *task;

#ifdef KTEST
  priority = 0;
  task = &TestTask;
#else
  priority = 3;
  #ifdef METRIC_64
    task = &K2InitMetricTask;
  #elif METRIC_4
    task = &K2InitMetricTask;
  #else
    task=&InitTask;
  #endif
#endif

  int tid = tt_get(&tid_tracker);
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

  ret = pq_dumb_pop(&pq_tasks, &td);
  KASSERT(ret == 0 && td != NULL);

  return td;
}

TaskRequest activate(TaskDescriptor* td) {
  // int reg;
  // int r0, r1, r2, r3, r4, r5, r6, r7, r8,r9, r10, r11, r12;
  // asm("mov r0, #0");
  // asm("mov r1, #1");
  // asm("mov r2, #2");
  // asm("mov r3, #3");
  // asm("mov r4, #4");
  // asm("mov r5, #5");
  // asm("mov r6, #6");
  // asm("mov r7, #7");
  // asm("mov r8, #8");
  // asm("mov %0, r9":"=r"(r9):);
  // asm("mov %0, r10":"=r"(r10):);
  // asm("mov %0, r11":"=r"(r11):);
  // asm("mov %0, r12":"=r"(r12):);

  // asm("stmfd sp!, {r8-r12};");
  // asm("stmfd sp!, {r0-r7};");
  // //asm("add sp, sp, #48");

  // asm("ldmfd sp!, {r0-r7}");


  // asm("mov %0, r0":"=r"(r0):);
  // asm("mov %0, r1":"=r"(r1):);
  // asm("mov %0, r2":"=r"(r2):);
  // asm("mov %0, r3":"=r"(r3):);
  // asm("mov %0, r4":"=r"(r4):);
  // asm("mov %0, r5":"=r"(r5):);
  // asm("mov %0, r6":"=r"(r6):);
  // asm("mov %0, r7":"=r"(r7):);

  // bwprintf(COM2, "%d\n\r", r0);
  // bwprintf(COM2, "%d\n\r", r1);
  // bwprintf(COM2, "%d\n\r", r2);
  // bwprintf(COM2, "%d\n\r", r3);
  // bwprintf(COM2, "%d\n\r", r4);
  // bwprintf(COM2, "%d\n\r", r5);
  // bwprintf(COM2, "%d\n\r", r6);
  // bwprintf(COM2, "%d\n\r", r7);

  // asm("ldmfd sp!, {r0-r4}");

  // asm("mov %0, r0":"=r"(r0):);
  // asm("mov %0, r1":"=r"(r1):);
  // asm("mov %0, r2":"=r"(r2):);
  // asm("mov %0, r3":"=r"(r3):);
  // asm("mov %0, r4":"=r"(r4):);
  // bwprintf(COM2, "%d\n\r", r0);
  // bwprintf(COM2, "%d\n\r", r1);
  // bwprintf(COM2, "%d\n\r", r2);
  // bwprintf(COM2, "%d\n\r", r3);
  // bwprintf(COM2, "%d\n\r", r4);
  // KASSERT(0);

  // POP_STACK("r0-r12");  


  //Store Kernel State
  //PUSH_STACK("r0-r12, lr");
  PUSH_STACK("r0-r12");
  // PUSH_STACK("lr");
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
  // POP_STACK("r0-r12, lr");
  // POP_STACK("lr");
  POP_STACK("r0-r12, lr");
  //Switch back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //Set r0 with the new return value from stack
  POP_STACK("r0");
  // PRINT_REG("sp");
  //Move to the user task
  REVERSE_SWI();

  // TODO: to avoid corruption of r12, could we save user state onto kernel
  //       stack?
  asm("IRQ_ENTRY:");
  //Disable the interrupt
  *(int *)(VIC1_BASE + VIC_SOFTINTCLEAR_OFFSET) = 0;
  *(int *)(VIC2_BASE + VIC_SOFTINTCLEAR_OFFSET) = 0;
  // PUSH_STACK("r12");
  // //Change to System mode
  // SET_CPSR(SYSTEM_MODE);
  // //Save the user state
  // PUSH_STACK("r0-r12, lr");
  // //Change to Kernel mode
  // SET_CPSR(IRQ_MODE);
  // //Save lr to stratch r3
  // asm("mov r3, lr");
  // //Change to System mode
  // SET_CPSR(SYSTEM_MODE);
  // //Save the lr(r3)
  // PUSH_STACK("r3")
  // //Change back to kernel mode
  // SET_CPSR(KERNEL_MODE);
  // //load the kernel stack (fp is now resuable again!)
  // POP_STACK("r0-r12");
  // //Change back to system mode
  // SET_CPSR(SYSTEM_MODE); //Note we can still use fp!
  // //Save the user sp to TaskDescriptor's sp
  // READ_SP(td->sp);
  // //Change back to kernel mode
  // SET_CPSR(KERNEL_MODE);
  // //Save the spsr to the TaskDescriptor's psr
  // READ_SPSR(td->psr);
  // //Load the user's r12 and put it on user stack
  // SET_CPSR(IRQ_MODE);
  // POP_STACK("r0");
  // asm("str r0, [%0, #52]"::"r"(td->sp)); 
  // // manually put swi arg in r0, avoid overhead of return
  // SET_CPSR(KERNEL_MODE);
  // SWI_ARG_FETCH("r0");
  // POP_STACK("lr");
  // asm("b ACTIVATE_END");

  //AFTER USER TASK CALLS SWI (CANT USE FP)
  asm("KERNEL_ENTRY:");
  // //Save the r12 to Kernel Stack - Implicitly changed by SET_CPSR
  // PUSH_STACK("r12");
  // //Change to System mode
  // SET_CPSR(SYSTEM_MODE);
  // //Save the user state
  // PUSH_STACK("r0-r12, lr");
  // //Change to Kernel mode
  // SET_CPSR(KERNEL_MODE);
  // //Save lr to stratch r3
  // asm("mov r3, lr");
  // //Change to System mode
  // SET_CPSR(SYSTEM_MODE);
  // //Save the lr(r3)
  // PUSH_STACK("r3")
  asm("stmfd sp, {r0-r12};");
  asm("sub r9, sp, #4;"
      "mov r10, lr;");
  //asm("add sp, sp, #52");
  //Change to System
  SET_CPSR(SYSTEM_MODE);
  // asm("add r9, r9, #-4");
  PUSH_STACK("lr");
  PRINT_REG("lr");
  asm("sub sp, sp, #4");
  asm("ldmda r9!, {r0-r7}");
  asm("stmda sp!, {r0-r7}");
  asm("ldmda r9!, {r0-r4}");
  // PRINT_REG("r4");
  asm("stmda sp!, {r0-r4}");
  asm("add sp, sp, #4");

  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  POP_STACK("r4");
  PRINT_REG("r4");
  KABORT();

  PUSH_STACK("r10");

  // //Change back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //load the kernel stack (fp is now resuable again!)
  //asm("add sp, sp, #4");
  //POP_STACK("r0-r12, lr");
  // POP_STACK("lr");
  POP_STACK("r0-r12");
  //Change back to system mode
  SET_CPSR(SYSTEM_MODE); //Note we can still use fp!
  //Save the user sp to TaskDescriptor's sp
  READ_SP(td->sp);
  //Change back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //Save the spsr to the TaskDescriptor's psr
  READ_SPSR(td->psr);
  // //Load the user's r12 and put it on user stack
  // asm("ldr r12, [sp, #-56]");
  // asm("str r12, [%0, #52]"::"r"(td->sp)); 
  // manually put swi arg in r0, avoid overhead of return
  //
  // IFF SWI
  SWI_ARG_FETCH("r0");
  //POP_STACK("lr");

  asm("ACTIVATE_END:");
  //KASSERT(0);
  return;
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

void get_parentTid( TaskDescriptor *td) {
  //Get the parent tid into user stack
  // asm(
  //   "str %0, [%1, #4]"::"r"(td->parent ? td->parent->tid : -1), "r"(td->sp)
  // );
  td->ret = td->parent ? td->parent->tid : -1;
}

void handle(TaskDescriptor *td, TaskRequest req) {
  SANITY();
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
  case SEND:
    send_handler(td);
    break;
  case RECEIVE:
    receive_handler(td);
    break;
  case REPLY:
    reply_handler(td);
    break;
  case NS_REG:
    ns_register(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case NS_GET:
    ns_get(td);
    pq_push(&pq_tasks, td->priority, td);
    break;
  case EXIT:
    KASSERT(0);
    // TODO: uninitialize the task descriptor
    td->status = ZOMBIE;
    tt_return(td->tid, &tid_tracker);
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
__attribute__((naked)) void main(void) {
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
  
#ifdef CACHE
  DISABLE_ALL_CACHE();
#endif

  KERNEL_EXIT();
}


