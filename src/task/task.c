#include <task.h>
#include <ts7200.h>

void tt_init(TidTracker *tt) {
  init_circularBuffer(&tt->cb);
  int i = 0;
  for(i = 0; i < MAX_TASK; i++) {
    tt->cb.buffer[tt->cb.buffer_end] = i;
    tt->cb.buffer_end = (tt->cb.buffer_end + 1) % CIRCULAR_BUFFER_SIZE;
  }
}

int tt_get(TidTracker *tt) {
  if(tt->cb.buffer_end == tt->cb.buffer_start) {
    return -2;
  }
  int tid = tt->cb.buffer[tt->cb.buffer_start];
  tt->cb.buffer_start = (tt->cb.buffer_start + 1) % CIRCULAR_BUFFER_SIZE;
  return tid;
}

void tt_return(int tid, TidTracker *tt) {
  tid += (1 << 16);
  tt->cb.buffer[tt->cb.buffer_end] = tid;
  tt->cb.buffer_end = (tt->cb.buffer_end + 1) % CIRCULAR_BUFFER_SIZE;
}

void td_init(TaskDescriptor *td) {
  td->tid = 0;
  td->sp =  0;
  td->psr = 0;
  td->task = NULL;
  td->status = UNINIT;
  td->stack_base = 0;
  td->parent = NULL;
  td->priority = 1;
}

/**
 * Copies the values of td2 into td1
 */
void td_copy(TaskDescriptor *td1, TaskDescriptor *td2) {
  td1->tid = td2->tid;
  td1->sp = td2->sp;
  td1->psr = td2->psr;
  td1->task = td2->task;
  td1->status = td2->status;
  td1->stack_base = td2->stack_base;
}
