#include <user/train/representer.h>
#include <user/train/interpreter.h>
#include <lib/train/estimator.h>

CIRCULAR_BUFFER_DEF(trm_subscribers, tid_t, MAX_EVENT_SUBSCRIBERS);

static tid_t tm_tid;

static void NotifySubscribers(trm_subscribers *subs, TrackEventType type, union TrackEvents *event) {
  int r;
  assert(type >= 0 && type < MAX_TRACK_EVENT);
  tid_t tid;
  trm_subscribers *event_subs;

  event_subs = &subs[type];
  while (event_subs->size > 0) {
    r = trm_subscribers_pop(event_subs, &tid);
    assert(r == 0);
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


static void ApplyUpdates(estimator *estimator, update_list *updates, trm_subscribers *subs) {
  int i, r;
  TrackEvent event;
  pos_event pe;

  assert(0 <= updates->size && updates->size < UPDATE_LIST_SIZE);

  for (i = 0; i < updates->size; ++i) {
    update_list_get(updates, i, &event);

    switch (event.type) {
      case TE_TR_SPEED:
        break;
      case TE_TR_STATUS:
        // TODO: relay to estimator
        // est_update_tr_status(estimator, &pe);
        break;
      case TE_TR_POSITION:
        pe.pos = event.event.tr_pos.node;
        assert(event.event.tr_pos.num >= 1 && event.event.tr_pos.num <= 80);
        assert(pe.pos == 1);
        assert(event.event.tr_pos.node == 1);
        TMPutStrf(tm_tid, "%d\n", pe.pos);
        // TMPutStrf(tm_tid, "%s\n", pe.pos->name);
        pe.ts  = event.ts;
        pe.off = 0;
        // r = est_update_tr_at(estimator, &pe);
        break;
      case TE_SE_CHANGE:
        // est_update_se(estimator, event.event.se_event.id, event.event.se_event.state);
        break;
      case TE_TR_MOVE:
        // r = est_update_tr_gear(estimator, event.event.tr_gear.num, event.event.tr_gear.newgear, event.ts);
        break;
      case TE_SW_CHANGE:
        r = est_update_sw(estimator, event.event.sw_change.num, event.event.sw_change.newdir, event.ts);
        break;
      default:
        assert(0);
    }

    // NotifySubscribers(subs, event.type, &event.event);
  }
}


#define POKE_TIME 50
static void Poke() {
  int r;
  tid_t par_tid, cs_tid, my_tid;
  TrackRequest req;

  my_tid = MyTid();
  par_tid = MyParentTid();
  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(my_tid > 0 && par_tid > 0 && cs_tid > 0);

  req.type = TRR_POKE;

  while (true) {
    req.data.time = Time(cs_tid, my_tid);
    Send(par_tid, &req, sizeof(req), &r, sizeof(r));
    Delay(cs_tid, my_tid, POKE_TIME);
  }
}

void Representer() {
  int i, r, track;
  tid_t req_tid, my_tid;
  TrackRequest req;
  estimator estimator;
  train *tr;

#ifdef TRACK_A
  track = 1;
#endif
#ifdef TRACK_B
  track = 2;
#endif

  assert(sizeof(estimator) < 200000);
  est_init(&estimator);

  my_tid = MyTid();
  est_init_trains(&estimator, TimeCS(my_tid), TRACK, track);

  r = RegisterAs(REPRESENTER_ID);
  assert(r == 0);

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  TMRegister(tm_tid, REP_OFF_X, REP_OFF_Y, REP_WIDTH, REP_HEIGHT);

  Create(27, &Interpreter);
  // Create(26, &Poke);

  trm_subscribers subscribers[MAX_TRACK_EVENT];
  for (i = 0; i < MAX_TRACK_EVENT; ++i) {
    trm_subscribers_init(&subscribers[i]);
  }

  // TMLogStrf(tm_tid, "test\n");

  while (true) {
    Receive(&req_tid, &req, sizeof(req));

    switch (req.type) {
      case TRR_POKE:
        assert(req.data.time > 0);
        est_update(&estimator, req.data.time);
        tr = est_get_train(&estimator, 1);
        if (tr) {
          // TMPutStrf(tm_tid, "\v%d %s %d", tr->num, tr->curr_pos.pos->name, tr->curr_pos.off);
        }
        Reply(req_tid, &r, sizeof(r));
        break;
      case TRR_UPDATE:
        //Reply(req_tid, &r, sizeof(r));
        ApplyUpdates(&estimator, &req.data.update, subscribers);
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
