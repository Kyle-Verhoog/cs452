#include <kernel_task.h>

void ktd_create(TaskDescriptor* volatile td, tid_t tid, void *task,
                int priority,
                TaskStatus status, TaskDescriptor *parent) {
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
  init_circularBuffer(&(td->send_q), td->buffer_q, 256);  //TODO: MAKE DEFINE

#ifdef TASK_METRICS
  td->start_time = *(int *)TM_CLOCK_VAL;
  td->running_time = 0;
#endif //TASK_METRICS
}
