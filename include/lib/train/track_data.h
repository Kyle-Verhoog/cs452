#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include <lib/string.h>
#include <lib/train/track_node.h>
// The track initialization functions expect an array of this size.
#define TRACK_MAX 144

extern track_node track_a[TRACK_MAX], track_b[TRACK_MAX], *TRACK;

void init_tracka(track_node *track);

void set_track_a();

void init_trackb(track_node *track);

void set_track_b();

int trhr(track_node *track, const char *);

#endif //TRACK_DATA_H
