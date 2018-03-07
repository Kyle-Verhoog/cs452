#include <task/task.h>

void tt_init(TidTracker *tt) {
  tid_cb_init(&tt->cb);
  int i = 0;
  int r;
  for(i = 0; i < MAX_TASK; i++) {
    r = tid_cb_push(&tt->cb, i);
    KASSERT(r == 0);
  }
}

tid_t tt_get(TidTracker *tt) {
  if(tt_size(tt) <= 0) {
    return -2;
  }
  tid_t tid;
  int r;
  r = tid_cb_pop(&tt->cb, &tid);
  KASSERT(r == 0);
  return tid;
}

void tt_return(tid_t tid,  TidTracker *tt) {
  int r;
  tid += (1 << 16);

  r = tid_cb_push(&tt->cb, tid);
  KASSERT(r == 0);
}

int tt_size(TidTracker *tt) {
  return (tt->cb).size;
}

void td_init(TaskDescriptor *td) {
  td->tid = 0;
  td->sp =  0;
  td->psr = 0;
  td->task = NULL;
  td->status = TS_UNINIT;
  td->it = IT_SWI;
  td->stack_base = 0;
  td->parent = NULL;
  td->priority = 1;
  tid_cb_init(&td->send_q);
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
