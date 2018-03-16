#include <user/train/representer.h>

CIRCULAR_BUFFER_DEF(trm_subscribers, tid_t, MAX_EVENT_SUBSCRIBERS);

static void NotifySubscribers(trm_subscribers *subs, TrackEventType type, union TrackEvents *event) {
  assert(type >= 0 && type < MAX_TRACK_EVENT);
  int i;
  tid_t tid;
  trm_subscribers *event_subs;

  event_subs = &subs[type];
  for (i = 0; i < subs->size; ++i) {
    trm_subscribers_get(event_subs, i, &tid);
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

static void ApplyUpdates(Track *track, TrackUpdate *updates, trm_subscribers *subs) {
  int i;
  TrackEvent *event;

  for (i = 0; i < updates->num; ++i) {
    event = &updates->events[i];
    switch (event->type) {
      case TE_TRAIN_MOVE:
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

  trm_subscribers subscribers[MAX_TRACK_EVENT];
  for (i = 0; i < MAX_TRACK_EVENT; ++i) {
    trm_subscribers_init(&subscribers[i]);
  }

  r = RegisterAs(REPRESENTER_ID);

  while (true) {
    Receive(&req_tid, &req, sizeof(req));

    switch (req.type) {
      case TRR_NOP:
        Reply(req_tid, &r, sizeof(r));
        break;
      case TRR_FETCH:
        Reply(req_tid, &track, sizeof(track));
        break;
      case TRR_UPDATE:
        Reply(req_tid, &r, sizeof(r));
        ApplyUpdates(&track, &req.data.update, subscribers);
        break;
      case TRR_SUBSCRIBE:
        AddSubscriber(subscribers, req_tid, req.type);
        break;
      default:
        assert(0);
        break;
    }
  }

  Exit();
}
