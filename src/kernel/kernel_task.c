#include <kernel_task.h>

void ktd_create(TaskDescriptor* volatile td, uint32_t tid, void *task,
                int priority,
                TaskStatus status, TaskDescriptor *parent) {
  //Initialize the Test task pc
  td->sp = USER_STACK_BASE - ((tid&0xffff)*USER_STACK_SIZE);
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
  init_circularBuffer(&(td->send_q));
}
