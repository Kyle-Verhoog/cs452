#include <lib/clockserver_queue.h>

void csq_node_init(csq_node *csqn, tid_id_t id) {
  csqn->id    = id;
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

int csq_add(cs_queue *csq, tid_id_t id, int ticks) {
  if (csq->size == CSQ_SIZE) return CSQ_FULL;
  
  csq_node *new  = &csq->nodes[id];
  new->ticks = ticks;
  csq_node *cur  =  csq->head;
  csq_node *prv  =  NULL;

  if (cur == NULL) {
    new->next = NULL;
    csq->head = new;
  }

  while (cur != NULL) {
    if (ticks < cur->ticks) {
      if (prv != NULL)
        prv->next = new;
      new->next = cur;
      break;
    }

    prv = cur;
    cur = cur->next;

    if (cur == NULL) {
      prv->next = new;
      new->next = NULL;
      break;
    }
  }

  csq->size++;
  return 0;
}

int csq_check(cs_queue *csq, int ticks) {
  if (ticks <= csq->head->ticks)
    return 1;
  return 0;
}

int csq_pop(cs_queue *csq, int ticks, tid_id_t *id) {
  KASSERT(csq->head != NULL);
  if (csq->head == NULL || ticks > csq->head->ticks)
    return CSQ_NREADY;
  
  *id = csq->head->id;
  csq->head = csq->head->next;
  csq->size--;
  return 0;
}

int csq_remove(cs_queue *csq, tid_id_t id) {
  return 0;
}

csq_node *csq_find(cs_queue *csq, tid_id_t id) {
  csq_node *cur;
  cur = csq->head;
  while (cur != NULL) {
    if (cur->id == id)
      return cur;
    cur = cur->next;
  }
  return (csq_node *)CSQ_NEXIST;
}

