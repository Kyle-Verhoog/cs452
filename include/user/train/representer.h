#ifndef TRACK_MANAGER_H
#define TRACK_MANAGER_H

#include <types.h>
#include <user/syscalls.h>
#include <user/nameserver.h>

#include <user/train/train_defines.h>
#include <user/train/track_event.h>

#define MAX_EVENT_SUBSCRIBERS 16

CIRCULAR_BUFFER_DEC(trm_subscribers, tid_t, MAX_EVENT_SUBSCRIBERS);

typedef struct Track {
  Train train[TRAIN_SIZE]; // TODO: make a list data structure
  char sensors[DECODER_SIZE*2];
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

union uTrackRequest {
  Track track;         // fetch
  TrackUpdate update;  // interpreter
  TrackEventType type; // subscribers
};

typedef struct TrackRequest {
  TrackRequestType type;
  union uTrackRequest data;
} TrackRequest;

void Representer();

#endif
