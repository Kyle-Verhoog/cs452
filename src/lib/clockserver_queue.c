#include <lib/clockserver_queue.h>

void csq_node_init(csq_node *csqn, tid_t tid) {
  csqn->tid   = tid;
  csqn->ticks = 0;
  csqn->next  = NULL;
}

void csq_init(cs_queue *csq) {
  csq->head = NULL;
  csq->size = 0;

  int i;
  for (i = 0; i < NUM_TASKS; i++) {
    csq_node_init(&csq->nodes[i], i);
  }
}

int csq_add(cs_queue *csq, tid_t id, int ticks) {
  KASSERT(csq != NULL);
  if (id > NUM_TASKS) return CSQ_E_TID;
  if (csq->size == CSQ_SIZE) return CSQ_E_FULL;
  
  csq_node *new = &csq->nodes[id];
  new->ticks    = ticks;
  csq_node *cur = csq->head;
  csq_node *prv = NULL;

  while (cur != NULL) {
    if (ticks < cur->ticks) {
      if (prv != NULL)
        prv->next = new;
      else
        csq->head = new;
      new->next = cur;
      goto END;
    }

    prv = cur;
    cur = cur->next;
  }

  if (prv != NULL)
    prv->next = new;
  else
    csq->head = new;
  new->next = NULL;

END:
  csq->size++;
  return 0;
}

int csq_check(cs_queue *csq, int ticks) {
  KASSERT(csq != NULL);
  if (ticks <= csq->head->ticks)
    return 1;
  return 0;
}

int csq_pop(cs_queue *csq, int ticks, tid_t *tid) {
  KASSERT(csq->head != NULL);
  if (csq->head == NULL)
    return CSQ_E_EMPTY;

  if (ticks < csq->head->ticks)
    return CSQ_E_READY;
  
  *tid = csq->head->tid;
  csq->head = csq->head->next;
  csq->size--;
  return 0;
}

int csq_remove(cs_queue *csq, tid_t tid) {
  KASSERT(0 && "TODO");
  return 0;
}

csq_node *csq_find(cs_queue *csq, tid_t tid) {
  csq_node *cur;
  cur = csq->head;
  while (cur != NULL) {
    if (cur->tid == tid)
      return cur;
    cur = cur->next;
  }

  return NULL;
}

