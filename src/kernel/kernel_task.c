#include <kernel/kernel_task.h>

void ktd_create(TaskDescriptor* volatile td, tid_t tid, void *task,
                int priority,
                TaskStatus status, TaskDescriptor *parent) {
  // KASSERT(tid)
  //Initialize the Test task pc
  td->sp = USER_STACK_BASE - (TID_ID(tid)*USER_STACK_SIZE);
  asm("mov r8, %0;"::"r"(task));
  asm("stmfd %0, {r8};"::"r"(td->sp));
  td->sp -= 60; //saved lr_svc

  td->tid = tid;
  td->psr = USER_MODE;
  td->task = task;
  td->status = status;
  td->it = IT_SWI;
  td->parent = parent;
  td->priority = priority;
  td->next = NULL;
  td->ret = 0;
  tid_cb_init(&(td->send_q));

  td->start_time = *(int *)TM_CLOCK_VAL;
  td->running_time = 0;
}


void ktd_create_args(TaskDescriptor* volatile td, tid_t tid, void *task,
                int priority, TaskStatus status, TaskDescriptor *parent,
                void *args, int argsize) {
  td->sp = USER_STACK_BASE - (TID_ID(tid)*USER_STACK_SIZE);

  if (argsize > 0) {
    td->sp -= argsize%4 == 0 ? argsize : 4*(argsize/4 + 1);
    // KASSERT(td->sp == USER_STACK_BASE - (TID_ID(tid)*USER_STACK_SIZE) - 4);
    // copy args to user task stack, shift task stack pointer down
    for (argsize = argsize-1; argsize >= 0; --argsize) {
      *((char *)(td->sp + argsize)) = *((char *)(args + argsize));
    }
    td->ret = (int)td->sp;
    td->sp -= 4; // sp points to next location
  }
  else {
    td->ret = 0;
  }

  asm("mov r8, %0;"::"r"(task));
  asm("stmfd %0, {r8};"::"r"(td->sp));
  td->sp -= 60; //saved lr_svc

  td->tid = tid;
  td->psr = USER_MODE;
  td->task = task;
  td->status = status;
  td->it = IT_SWI;
  td->parent = parent;
  td->priority = priority;
  td->next = NULL;
  tid_cb_init(&(td->send_q));

  td->start_time = *(int *)TM_CLOCK_VAL;
  td->running_time = 0;
}
