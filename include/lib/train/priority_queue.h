#ifndef TRAIN_P_QUEUE_H
#define TRAIN_P_QUEUE_H

#include <defines.h>
#include <lib/train/track_data.h>

#define MAX_TPQ_SIZE TRACK_MAX


typedef struct train_priority_queue_node {
  int priority;
  int val;
} tp_queue_node;

typedef struct train_priority_queue {
  tp_queue_node tree[MAX_TPQ_SIZE];
  int map[TRACK_MAX];
  int size;
} tp_queue;


void tpq_init(tp_queue *tpq);

int tpq_add(tp_queue *tpq, int key, int priority);

int tpq_pop(tp_queue *tpq);

int tpq_dec_priority(tp_queue *tpq, int val, int priority);
#endif
