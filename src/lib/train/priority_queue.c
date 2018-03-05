#include <lib/train/priority_queue.h>

#define parent(i) (i-1)/2
#define left(i) (2*i+1)
#define right(i) (2*i+2)
#define is_null(n) (n.priority == INT_MAX)


void tpq_init(tp_queue *tpq) {
  int i;
  tpq->size = 0;
  for (i = 0; i < MAX_TPQ_SIZE; ++i) {
    tpq->tree[i].priority = INT_MAX;
    tpq->tree[i].val = 0;
  }
}


int tpq_add(tp_queue *tpq, int val, int priority) {
  int i, p;
  tp_queue_node *tree, t;
  int *map;
  tree = tpq->tree;
  map = tpq->map;

  i = tpq->size;
  tree[i].val = val;
  tree[i].priority = priority;
  map[val] = i;

  p = parent(i);
  while (p > -1 && tree[p].priority > tree[i].priority) {
    t = tree[p];
    tree[p] = tree[i];
    tree[i] = t;

    map[tree[i].val] = i;
    map[tree[p].val] = p;
    i = p;
    p = parent(i);
  }

  tpq->size++;
  return 0;
}


int tpq_pop(tp_queue *tpq) {
  int i, l, r;
  tp_queue_node *tree, ret, t;
  int *map;

  tree = tpq->tree;
  map = tpq->map;
  ret = tree[0];
  i = tpq->size-1;

  tree[0] = tree[i];
  tree[i].val = 0;
  tree[i].priority = INT_MAX;

  i = 0;
  while (1) {
    l = left(i);
    if (tree[l].priority < tree[i].priority && !is_null(tree[l])) {
      t = tree[i];
      tree[i] = tree[l];
      tree[l] = t;
      map[tree[i].val] = i;
      map[tree[l].val] = l;
      i = l;
      continue;
    }
    r = right(i);
    if (tree[r].priority < tree[i].priority && !is_null(tree[r])) {
      t = tree[i];
      tree[i] = tree[r];
      tree[r] = t;
      map[tree[i].val] = i;
      map[tree[r].val] = r;
      i = r;
      continue;
    }
    break;
  }

  tpq->size--;
  return ret.val;
}

int tpq_dec_priority(tp_queue *tpq, int val, int priority) {
  int i, p;
  tp_queue_node *tree, t;
  int *map;

  map = tpq->map;
  tree = tpq->tree;

  i = map[val];
  tree[i].priority = priority;
  p = parent(i);

  while (p > -1 && tree[p].priority > tree[i].priority) {
    t = tree[p];
    tree[p] = tree[i];
    tree[i] = t;

    map[tree[i].val] = i;
    map[tree[p].val] = p;
    i = p;
    p = parent(i);
  }

  return 0;
}
