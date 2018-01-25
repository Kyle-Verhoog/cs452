#include <kernel_task.h>
#include <bwio.h>

void ktd_create(volatile TaskDescriptor *td, uint32_t tid, volatile void *task, int priority,
                TaskStatus status, volatile TaskDescriptor *parent) {
  //Initialize the Test task pc
  td->sp = USER_STACK_BASE - (tid*USER_STACK_SIZE) - 56;
  asm("mov r8, %0;"::"r"(task));
  asm("stmfd %0!, {r8};"::"r"(td->sp));
  td->sp -= 4; //saved lr_svc

  td->tid = tid;
  td->psr = USER_MODE;
  td->task = task;
  td->status = status;
  td->parent = parent;
  td->priority = priority;
  td->next = NULL;
  td->ret = 0;
}
