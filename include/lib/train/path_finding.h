#ifndef PATH_FINDING_H
#define PATH_FINDING_H

#include <defines.h>
#include <lib/circular_buffer.h>
#include <lib/train/track_node.h>
#include <lib/train/track_data.h>
#include <lib/train/priority_queue.h>

CIRCULAR_BUFFER_DEC(tr_path, track_node*, TRACK_MAX);
CIRCULAR_BUFFER_DEC(sw_look_ahead, track_node*, TRACK_MAX);

typedef struct train_path {
  bool active;
  bool ready;
  track_node *track;
  track_node *start;
  track_node *end;
  track_node *current;
  tr_path ahead;
  tr_path behind;     // should probably be a stack instead
  int pred[TRACK_MAX];
} path;

void path_init(path *p, track_node *track);


#ifndef X86
#include <user/train/train_manager.h>
int generate_train_path(TrainDescriptor *tr, track_node *track, int sid, int eid);
#endif

void dij_path_find(track_node *track, track_node *s, track_node *d, int *prev);

void path_init(path *p, track_node *track);

void path_next(path *p);

void path_set_destination(path *p, track_node *start, track_node *end);

#endif
