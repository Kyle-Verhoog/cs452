#include <task.h>
#include <ts7200.h>

void tt_init( TidTracker *tt) {
  init_circularBuffer(&tt->cb);
  int i = 0;
  for(i = 0; i < MAX_TASK; i++) {
    push_circularBuffer(&tt->cb, i);
  }
}

int tt_get( TidTracker *tt) {
  if(tt->cb.buffer_end == tt->cb.buffer_start) {
    return -2;
  }
  int tid = top_circularBuffer(&tt->cb);
  pop_circularBuffer(&tt->cb);
  return tid;
}

void tt_return(int tid,  TidTracker *tt) {
  tid += (1 << 16);

  push_circularBuffer(&tt->cb, tid);
}

void td_init( TaskDescriptor *td) {
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
void td_copy( TaskDescriptor *td1,  TaskDescriptor *td2) {
  td1->tid = td2->tid;
  td1->sp = td2->sp;
  td1->psr = td2->psr;
  td1->task = td2->task;
  td1->status = td2->status;
  td1->stack_base = td2->stack_base;
}
