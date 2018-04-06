#ifndef TRACK_MANAGER_H
#define TRACK_MANAGER_H

#include <types.h>
#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/clockserver.h>

#include <lib/train/track.h>

#define MAX_EVENT_SUBSCRIBERS 16

CIRCULAR_BUFFER_DEC(trm_subscribers, tid_t, MAX_EVENT_SUBSCRIBERS);
CIRCULAR_BUFFER_DEC(tr_subscribers, tid_t, MAX_EVENT_SUBSCRIBERS);


typedef enum TrackRequestType {
  TRR_SUBSCRIBE = 0,  // subscribe on an event
  TRR_FETCH     = 1,  // get all the track data
  TRR_UPDATE    = 2,  // update the track model given a list of updates
  TRR_POKE      = 3,  // trigger an estimator update
} TrackRequestType;

union uTrackRequest {
  update_list update;  // interpreter
  TrackEventType type; // subscribers
  TrackDataType dtype; // subscribers
  int time;
  int train_num;
};

typedef struct TrackRequest {
  TrackRequestType type;
  union uTrackRequest data;
} TrackRequest;

void Representer();

#endif
