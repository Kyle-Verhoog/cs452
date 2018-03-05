#ifndef PATH_FINDING_H
#define PATH_FINDING_H

#include <defines.h>
#include <lib/train/track_node.h>
#include <lib/train/track_data.h>
#include <lib/train/priority_queue.h>


void bf_path_find(track_node *s, track_node *d);

void dij_path_find(track_node *track, track_node *s, track_node *d, int *prev);

#endif