#include <task/task.h>
#include <ts7200.h>

void tt_init( TidTracker *tt) {
  init_circularBuffer(&tt->cb, tt->buffer_tid, MAX_TASK);
  int i = 0;
  for(i = 0; i < MAX_TASK; i++) {
    push_circularBuffer(&tt->cb, i);
  }
}

tid_t tt_get(TidTracker *tt) {
  if(tt_size(tt) == 0) {
    return -2;
  }
  tid_t tid = top_circularBuffer(&tt->cb);
  pop_circularBuffer(&tt->cb);
  return tid;
}

void tt_return(tid_t tid,  TidTracker *tt) {
  tid += (1 << 16);

  push_circularBuffer(&tt->cb, tid);
}

int tt_size(TidTracker *tt) {
  return (tt->cb).size;
}

void td_init(TaskDescriptor *td) {
  td->tid = 0;
  td->sp =  0;
  td->psr = 0;
  td->task = NULL;
  td->status = UNINIT;
  td->it = IT_SWI;
  td->stack_base = 0;
  td->parent = NULL;
  td->priority = 1;
  init_circularBuffer(&td->send_q, td->buffer_q, MAX_TASK);
}

#ifdef TASK_METRICS
  // TODO: MAKE THIS STATIC ARRAY BETTER
  TaskSummary TASK_SUMMARY[2056];
  int SUMMARY_HEAD;

  void tm_init(){
    *(int *)TM_CLOCK_LDR = TM_CLOCK_VALUE;
    *(int *)TM_CLOCK_CTRL = TM_CLOCK_FLAGS;

    SUMMARY_HEAD = 0;
  }

  void tm_delta(int st, int et, TaskDescriptor *td){
    int delta;
    if(et > st){  //handle wrap around
      delta = st + TM_CLOCK_VALUE - et;
    }
    else{
      delta = st - et;
    }

    td->running_time += delta;
    td->end_time = et;
  }

  void tm_addSummary(TaskDescriptor *td){
    KASSERT(SUMMARY_HEAD != 2056);

    TASK_SUMMARY[SUMMARY_HEAD].tid = td->tid;
    TASK_SUMMARY[SUMMARY_HEAD].priority = td->priority;
    TASK_SUMMARY[SUMMARY_HEAD].start_time = td->start_time;
    TASK_SUMMARY[SUMMARY_HEAD].running_time = td->running_time;
    TASK_SUMMARY[SUMMARY_HEAD].end_time = td->end_time;

    SUMMARY_HEAD++;
  }

  void tm_summarize(){
    int i;
    for(i = 0; i < SUMMARY_HEAD; i++){
      PRINTF("Task %x ran for %d of %d (%d%c) ticks.\n\r", 
        TASK_SUMMARY[i].tid, 
        TASK_SUMMARY[i].running_time,
        (TASK_SUMMARY[i].start_time - TASK_SUMMARY[i].end_time),
        TASK_SUMMARY[i].running_time*100 / (TASK_SUMMARY[i].start_time - TASK_SUMMARY[i].end_time),
        37);
    }
  }
#endif //TASK_METRICS
