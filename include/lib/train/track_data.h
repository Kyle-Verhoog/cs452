/* THIS FILE IS GENERATED CODE -- DO NOT EDIT */
#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include <lib/string.h>
#include <lib/train/track_node.h>

// The track initialization functions expect an array of this size.
#define TRACK_MAX 144

void init_tracka(track_node *track);
void init_trackb(track_node *track);

int trhr(track_node *track, const char *);

#endif //TRACK_DATA_H
