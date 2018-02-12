#include <kernel/handlers/create.h>

void create(TaskDescriptor *td) {
  //Get the arguments r0 (priority) r1 (function pointer)
  tid_t tid = tt_get(&tid_tracker);
  int priority;
  void *task;

  asm("ldr %0, [%1, #4];":"=r"(priority):"r"(td->sp));
  asm("ldr %0, [%1, #8];":"=r"(task):"r"(td->sp));
  KASSERT(IS_VALID_PRIORITY(priority));

  if (tid < 0) {
    td->ret = -2;
    KASSERT(false && "Out of Tids");
  }
  else {
    TaskDescriptor *newTask = &tasks[TID_ID(tid)];
    ktd_create(newTask, tid, task, priority, TS_READY, td);
    pq_push(&pq_tasks, priority, newTask);
    td->ret = tid;
  }
}

void create_w_args(TaskDescriptor *td) {
  //Get the arguments r0 (priority) r1 (function pointer)
  tid_t tid = tt_get(&tid_tracker);
  int priority;
  void *task;
  void *args;

  asm("ldr %0, [%1, #4];":"=r"(priority):"r"(td->sp));
  asm("ldr %0, [%1, #8];":"=r"(task):"r"(td->sp));
  asm("ldr %0, [%1, #12];":"=r"(args):"r"(td->sp));
  KASSERT(IS_VALID_PRIORITY(priority));

  if (tid < 0) {
    td->ret = -2;
    KASSERT(false && "Out of Tids");
  }
  else {
    TaskDescriptor *newTask = &tasks[TID_ID(tid)];
    ktd_create(newTask, tid, task, priority, TS_READY, td);
    newTask->ret = (int)args;
    pq_push(&pq_tasks, priority, newTask);
    td->ret = tid;
  }
}
