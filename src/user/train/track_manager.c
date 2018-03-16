#include <user/train/track_manager.h>

void ApplyUpdates(Track *track, TrackUpdate *updates) {
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
  }
}

void Representer() {
  int r;
  tid_t req_tid;
  TrackRequest req;
  Track track;

  r = RegisterAs(REPRESENTER_ID);

  while (true) {
    Receive(&req_tid, &req, sizeof(req));

    switch (req.type) {
      case TRR_FETCH:
        Reply(req_tid, &track, sizeof(track));
        break;
      case TRR_UPDATE:
        ApplyUpdates(&track, &req.update);
        break;
      case TRR_SUBSCRIBE:
        break;
      default:
        assert(0);
        break;
    }
  }

  Exit();
}
