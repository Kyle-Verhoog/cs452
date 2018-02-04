#ifndef CLOCKSERVER_QUEUE_H
#define CLOCKSERVER_QUEUE_H

#include <stdlib.h>

#define CSQ_SIZE 50

#define CSQ_NONE    0
#define CSQ_E_FULL  1
#define CSQ_E_EMPTY 2
#define CSQ_E_EXIST 3
#define CSQ_E_READY 4
#define CSQ_E_TID   5

typedef struct clockserver_queue_node {
  tid_t tid;
  uint32_t ticks;
  struct clockserver_queue_node *next;
} csq_node;

void csq_node_init(csq_node *csqn, tid_t id);


typedef struct clockserver_queue {
  int size;
  csq_node nodes[NUM_TASKS];
  csq_node *head;
} cs_queue;


void csq_init(cs_queue *csq);

int csq_add(cs_queue *csq, tid_t tid, int ticks);

int csq_check(cs_queue *csq, int ticks);

int csq_pop(cs_queue *csq, int ticks, tid_t *tid);

int csq_remove(cs_queue *csq, tid_t tid);

csq_node *csq_find(cs_queue *csq, tid_t tid);

#endif /* CLOCKSERVER_QUEUE_H */
