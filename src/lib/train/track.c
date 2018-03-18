#include <lib/train/track.h>

CIRCULAR_BUFFER_DEF(train_cb, Train, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEF(train_list, Train *, TRAIN_LIST_SIZE);
CIRCULAR_BUFFER_DEF(poss_node_list, track_node *, POSSIBLE_NODE_LIST_SIZE);

void TrackInit(Track *track) {
  int i;

  train_cb_init(&track->all_trains);
  train_list_init(&track->orphan_trains);
  train_list_init(&track->active_trains);

  Train *train;
  Train t;
  t.num = 24;
  t.speed = 0;
  t.pos = &TRACK[4];
  t.timestamp = 0;
  train_cb_push(&track->all_trains, t);
  train = &track->all_trains.buf[0];
  train_list_push(&track->orphan_trains, train);
  //assert(&track->orphan_trains.size == 1);

  for (i = 0; i < DECODER_SIZE*2; ++i) {
    track->sensors[i] = 0;
  }
}

// TODO: replace with proper list datastructure
Train *GetActiveTrain(Track *track, int train_num) {
  int i, r;
  Train *train;

  for (i = 0; i < track->active_trains.size; ++i) {
    r = train_list_get(&track->active_trains, i, &train);
    assert(r == 0);

    if (train->num == train_num)
      return train;
  }

  return 0;
}

// TODO: replace with proper list datastructure
Train *RemoveActiveTrain(Track *track, int train_num) {
  int i, r, size;
  Train *train;

  size = track->active_trains.size;
  for (i = 0; i < size; ++i) {
    r = train_list_pop(&track->active_trains, &train);
    assert(r == 0);

    if (train->num == train_num)
      return train;

    r = train_list_push(&track->active_trains, train);
    assert(r == 0);
  }

  return 0;
}


int TrackGetNextPossibleSensors(Track *track, track_node *node, poss_node_list *pnl) {
  int r;
  assert(node != NULL);
  assert(node->type == NODE_SENSOR);

  node = node->edge[DIR_AHEAD].dest;

  while (node->type != NODE_SENSOR) {
    if (node->type == NODE_BRANCH) {
      TrackGetNextPossibleSensors(track, node->edge[DIR_STRAIGHT].dest, pnl);
      TrackGetNextPossibleSensors(track, node->edge[DIR_CURVED].dest, pnl);
      return 0;
    }
    else if (node->type == NODE_EXIT) {
      return 0;
    }
    node = node->edge[DIR_AHEAD].dest;
  }

  assert(node->type == NODE_SENSOR);
  r = poss_node_list_push(pnl, node);
  assert(r == 0);
  return 0;
}


