#include <user/train/interpreter.h>

static tid_t tm_tid;

static void DispatchVirtualEvents(tid_t vep_tid, ve_list *events) {
  int r;
  VERequest ver;
  VirtualEvent ve;

  ver.type = VER_REGISTER;

  while (events->size > 0) {
    r = ve_list_pop(events, &ve);
    assert(r == 0);

    ver.ve = ve;
    Send(vep_tid, &ver, sizeof(ver), &r, sizeof(r));
  }
}

static void DispatchTrackUpdates(tid_t rep_tid, update_list *update) {
  int r;
  TrackRequest rep_req;

  rep_req.type = TRR_UPDATE;
  rep_req.data.update = *update;
  Send(rep_tid, &rep_req, sizeof(rep_req), &r, sizeof(r));
  update_list_init(update);
}

static void WaitingRoomCourier() {
  int r;
  tid_t int_tid, wr_tid;
  WRRequest wrr;
  EventGroup group;

  int_tid = MyParentTid();
  assert(int_tid > 0);

  wr_tid = WhoIs(WAITING_ROOM_ID);
  assert(wr_tid > 0);

  wrr.type = WR_CE;
  while (true) {
    Send(wr_tid, &wrr, sizeof(wrr), &group, sizeof(group));
    Send(int_tid, &group, sizeof(group), &r, sizeof(r));
  }
}

void Interpreter() {
  int r;
  tid_t rep_tid, vep_tid;
  Track track;

  TrackInit(&track, TRACK);

  Train test;
  test.num = 24;
  TrackAddLostTrain(&track, &test);

  rep_tid = WhoIs(REPRESENTER_ID);
  vep_tid = WhoIs(VIRTUAL_PROVIDER_ID);
  tm_tid  = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0 && rep_tid > 0 && vep_tid > 0);

  // Subscribers to data publishers
  Create(29, &WaitingRoomCourier);

  EventGroup group;
  tid_t req_tid;
  while (true) {
    Receive(&req_tid, &group, sizeof(group));
    TrackInterpretEventGroup(&track, &group);
    DispatchVirtualEvents(vep_tid, &track.vevents);
    DispatchTrackUpdates(rep_tid, &track.updates);
    Reply(req_tid, &r, sizeof(r));
  }

  Exit();
}


