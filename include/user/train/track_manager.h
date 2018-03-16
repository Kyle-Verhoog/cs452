#ifndef TRACK_MANAGER_H
#define TRACK_MANAGER_H

#include <types.h>
#include <user/syscalls.h>
#include <user/nameserver.h>

#include <user/train/train_defines.h>
#include <user/train/track_event.h>

typedef struct Track {
  TrainDescriptor train[TRAIN_SIZE]; // TODO: make a list data structure
  // trains..
  //  - locations
  //  - destinations
  //  - paths
  //  - speed information
  // switch configurations
} Track;

typedef enum TrackRequestType {
  TRR_SUBSCRIBE = 0,  // subscribe on an event
  TRR_FETCH     = 1,  // get all the track data
  TRR_UPDATE    = 2,  // update the track model given a list of updates
} TrackRequestType;

typedef struct TrackRequest {
  TrackRequestType type;
  Track track;
  TrackUpdate update;
} TrackRequest;

void Representer();

#endif
