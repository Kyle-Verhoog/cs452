#ifndef TRACK_MANAGER_H
#define TRACK_MANAGER_H

#include <types.h>
#include <user/syscalls.h>
#include <user/nameserver.h>

#include <lib/train/track.h>
#include <lib/train/train.h>
#include <user/train/updates.h>

#define MAX_EVENT_SUBSCRIBERS 16

CIRCULAR_BUFFER_DEC(trm_subscribers, tid_t, MAX_EVENT_SUBSCRIBERS);


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
