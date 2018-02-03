#ifndef CLOCKSERVER_QUEUE_H
#define CLOCKSERVER_QUEUE_H

#include <types.h>

#define CSQ_SIZE 50

typedef struct clockserver_queue_node {
  struct clockserver_queue_node *next;
  tid_t tid;
} csq_node;


typedef struct clockserver_queue {
  int size;
  csq_node *head;
} cs_queue;


void csq_init(cs_queue *csq);

int csq_add(cs_queue *csq, tid_t tid, int ticks);

int csq_check(cs_queue *csq, int ticks);

int csq_pop(cs_queue *csq, int ticks, tid_t *tid);

#endif /* CLOCKSERVER_QUEUE_H */
