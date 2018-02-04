#ifndef CLOCKSERVER_QUEUE_H
#define CLOCKSERVER_QUEUE_H

#include <stdlib.h>

#define CSQ_SIZE 50

#define CSQ_NONE   0
#define CSQ_FULL   1
#define CSQ_EMPTY  2
#define CSQ_NEXIST 3
#define CSQ_NREADY 4

typedef struct clockserver_queue_node {
  tid_id_t id;
  uint32_t ticks;
  struct clockserver_queue_node *next;
} csq_node;

void csq_node_init(csq_node *csqn, tid_id_t id);


typedef struct clockserver_queue {
  int size;
  csq_node nodes[NUM_TASKS];
  csq_node *head;
} cs_queue;


void csq_init(cs_queue *csq);

int csq_add(cs_queue *csq, tid_id_t id, int ticks);

int csq_check(cs_queue *csq, int ticks);

int csq_pop(cs_queue *csq, int ticks, tid_id_t *id);

int csq_remove(cs_queue *csq, tid_id_t id);

csq_node *csq_find(cs_queue *csq, tid_id_t id);

#endif /* CLOCKSERVER_QUEUE_H */
