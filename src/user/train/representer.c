#include <user/train/representer.h>
#include <user/train/interpreter.h>

CIRCULAR_BUFFER_DEF(trm_subscribers, tid_t, MAX_EVENT_SUBSCRIBERS);

static void NotifySubscribers(trm_subscribers *subs, TrackEventType type, union TrackEvents *event) {
  assert(type >= 0 && type < MAX_TRACK_EVENT);
  tid_t tid;
  trm_subscribers *event_subs;

  event_subs = &subs[type];
  while (event_subs->size > 0) {
    trm_subscribers_pop(event_subs, &tid);
    Reply(tid, event, sizeof(union TrackEvents));
  }
}

static void AddSubscriber(trm_subscribers *subs, tid_t tid, TrackEventType type) {
  assert(type >= 0 && type < MAX_TRACK_EVENT);
  int r;
  trm_subscribers *event_subs;

  event_subs = &subs[type];
  r = trm_subscribers_push(event_subs, tid);
  assert(r == 0);
}

static void ApplySensorChange(Track *track, TESEChange *event) {
  track->sensors[event->num].state = event->state;
}

static void ApplySwitchChange(Track *track, TESWChange *event){
  track->switches[event->num].state = event->newdir;
}

static void ApplyUpdates(Track *track, update_list *updates, trm_subscribers *subs) {
  int i;
  TrackEvent event;

  if (!(0 <= updates->size && updates->size < UPDATE_LIST_SIZE)) {
    PRINTF("%d\n", updates->size);
  }
  assert(0 <= updates->size && updates->size < UPDATE_LIST_SIZE);

  for (i = 0; i < updates->size; ++i) {
    update_list_get(updates, i, &event);

    switch (event.type) {
      case TE_SE_CHANGE:
        ApplySensorChange(track, &event.event.se_change);
        break;
      case TE_TR_MOVE:
        //assert(0 && "TODO");
        break;
      case TE_SW_CHANGE:
        // assert(0 && "TODO");
        ApplySwitchChange(track, &event.event.sw_change);
        break;
      default:
        assert(0);
    }

    NotifySubscribers(subs, event.type, &event.event);
  }
}

static void ReplyByDataType(tid_t req_tid, union uTrackRequest data, Track *track){
  switch(data.dtype){
    case TD_ALL:
      Reply(req_tid, track, sizeof(Track));
      break;
    case TD_TR_A:
      Reply(req_tid, &track->active_trains, sizeof(track->active_trains));
      break;
    case TD_TR_L:
      Reply(req_tid, &track->lost_trains, sizeof(track->lost_trains));
      break;
    case TD_SW:
      Reply(req_tid, &track->switches, sizeof(track->switches));
      break;
    case TD_SE:
      Reply(req_tid, &track->sensors, sizeof(track->sensors));
      break;
    default:
      assert(0 && "Bad Data Request");
  }
}

void Representer() {
  int i, r;
  tid_t req_tid;
  TrackRequest req;
  Track track;

  r = RegisterAs(REPRESENTER_ID);
  assert(r == 0);

  TrackInit(&track, TRACK);

  Create(27, &Interpreter);

  trm_subscribers subscribers[MAX_TRACK_EVENT];
  for (i = 0; i < MAX_TRACK_EVENT; ++i) {
    trm_subscribers_init(&subscribers[i]);
  }

  while (true) {
    Receive(&req_tid, &req, sizeof(req));

    switch (req.type) {
      case TRR_FETCH:
        ReplyByDataType(req_tid, req.data, &track);
        //Reply(req_tid, &track, sizeof(track));
        break;
      case TRR_UPDATE:
        ApplyUpdates(&track, &req.data.update, subscribers);
        Reply(req_tid, &r, sizeof(r));
        break;
      case TRR_SUBSCRIBE:
        AddSubscriber(subscribers, req_tid, req.data.type);
        break;
      default:
        assert(0);
        break;
    }
  }
  
  Exit();
}
