#include <user/train/representer.h>
#include <user/train/interpreter.h>

CIRCULAR_BUFFER_DEF(trm_subscribers, tid_t, MAX_EVENT_SUBSCRIBERS);

static void TrackInit(Track *track) {
  int i;

  for (i = 0; i < DECODER_SIZE*2; ++i) {
    track->sensors[i] = 0;
  }
}

static void NotifySubscribers(trm_subscribers *subs, TrackEventType type, union TrackEvents *event) {
  assert(type == TE_SE_CHANGE);
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

  assert(type == TE_SE_CHANGE);
  event_subs = &subs[type];
  r = trm_subscribers_push(event_subs, tid);
  assert(r == 0);
}

static void ApplySensorChange(Track *track, TESEChange *event) {
  char *sensors;

  sensors = track->sensors;
  TOGGLE_SENSOR(sensors, event->dec, event->sen);
}

static void ApplyUpdates(Track *track, TrackUpdate *updates, trm_subscribers *subs) {
  int i;
  TrackEvent *event;

  assert(updates->num < MAX_NUM_UPDATES);

  for (i = 0; i < updates->num; ++i) {
    event = &updates->events[i];
    switch (event->type) {
      case TE_SE_CHANGE:
        ApplySensorChange(track, &event->event.se_change);
        break;
      case TE_TR_MOVE:
        //assert(0 && "TODO");
        break;
      case TE_SW_CHANGE:
        assert(0 && "TODO");
        break;
      default:
        assert(0);
    }

    NotifySubscribers(subs, event->type, &event->event);
  }
}

void Representer() {
  int i, r;
  tid_t req_tid;
  TrackRequest req;
  Track track;

  r = RegisterAs(REPRESENTER_ID);
  assert(r == 0);

  TrackInit(&track);

  Create(27, &Interpreter);

  trm_subscribers subscribers[MAX_TRACK_EVENT];
  for (i = 0; i < MAX_TRACK_EVENT; ++i) {
    trm_subscribers_init(&subscribers[i]);
  }

  while (true) {
    Receive(&req_tid, &req, sizeof(req));

    switch (req.type) {
      case TRR_FETCH:
        Reply(req_tid, &track, sizeof(track));
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
