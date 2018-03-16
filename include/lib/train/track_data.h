#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include <lib/string.h>
#include <lib/train/track_node.h>
// The track initialization functions expect an array of this size.
#define TRACK_MAX 144

extern track_node track_a[TRACK_MAX], track_b[TRACK_MAX], *track;

void init_tracka();

void set_track_a();

void init_trackb();

void set_track_b();

int trhr(track_node *track, const char *);

#endif //TRACK_DATA_H
