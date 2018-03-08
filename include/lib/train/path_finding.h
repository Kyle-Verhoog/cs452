#ifndef PATH_FINDING_H
#define PATH_FINDING_H

#include <stdlib.h>
#include <defines.h>
#include <lib/buffer_pack.h>
#include <lib/circular_buffer.h>
#include <lib/train/track_node.h>
#include <lib/train/track_data.h>
#include <lib/train/priority_queue.h>

#ifdef X86
#include <assert.h>
#endif


CIRCULAR_BUFFER_DEC(tr_path, track_node*, TRACK_MAX);

typedef struct train_path {
  bool active;
  bool ready;
  bool done;
  uint32_t path_len;
  track_node *track;
  track_node *start;
  track_node *end;
  track_node *current;
  tr_path ahead;
  tr_path behind;     // should probably be a stack instead
  int pred[TRACK_MAX];
} path;

typedef struct switch_config {
  track_node *sw;
  int state_required;
} sw_config;

CIRCULAR_BUFFER_DEC(sw_configs, sw_config, TRACK_MAX);

void path_init(path *p, track_node *track);

void dij_path_find(track_node *track, track_node *s, track_node *d, int *prev);

void path_init(path *p, track_node *track);

void path_start(path *p, track_node *start_node);

int path_generate(path *p);

void path_next(path *p);

void path_to_str(path *p, char *buf);

void path_switches_in_next_dist(path *p, sw_configs *sw_cfgs, int dist);

void path_set_destination(path *p, track_node *start, track_node *end);

int path_follow_to(path *p, track_node *t);

int path_arrived(path *p);

int track_node_dist(track_node *t1, track_node *t2);

#endif
