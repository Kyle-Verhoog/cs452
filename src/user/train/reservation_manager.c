#include <user/train/reservation_manager.h>

static tid_t tm_tid;

static RMReply HandleReserve(pather *p, int tr_num, track_node *node, int dist) {
  RMReply rep;
  rep.ret = pather_reserve_to_sensor(p, tr_num, node, dist);
  // if (rep.ret) {
  //   TMLogStrf(tm_tid, "RESERV FAILED\n");
  // }
  return rep;
}

static RMReply HandleFree(pather *p, int tr_num, track_node *node) {
  RMReply rep;

  rep.ret = pather_free_before(p, tr_num, node);

  // if (rep.ret) {
  //   TMLogStrf(tm_tid, "FREE HAD A PROBLEM: %d\n", rep.ret);
  // }
  return rep;
}

static void PrintReservationData(pather *p) {
  int i, j, offset;
  bool exist;
  char buf[8192];

  offset = buf_pack_c(buf, '\r');
  for (i = 0; i < TRAIN_MAX; ++i) {
    exist = false;
    for (j = 0; j < TRACK_MAX; ++j) {
      if (i == p->reserv[j].train_num && p->reserv[j].reserved) {
        if (!exist) {
          exist = true;
          offset += buf_pack_f(buf+offset, "tr %d: ", i);
        }
        offset += buf_pack_f(buf+offset, " %s", TRACK[j].name);
      }
    }
    if (exist)
      offset += buf_pack_c(buf+offset, '\n');
  }

  TMPutStr(tm_tid, buf, offset);
}

void ReservationManager() {
  int r;
  tid_t req_tid;
  pather reserv;
  RMReq req;
  RMReply rep;

  pather_init(&reserv, TRACK);

  r = RegisterAs(RESERVATION_MANAGER_ID);
  assert(r == 0);

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  TMRegister(tm_tid, RESERV_OFF_X, RESERV_OFF_Y, RESERV_WIDTH, RESERV_HEIGHT);
  TMPutStrf(tm_tid, "reservations\n");

  while (true) {
    Receive(&req_tid, &req, sizeof(req));

    switch (req.type) {
      case RM_RESERVE:
        rep = HandleReserve(&reserv, req.tr_num, req.node, req.dist);
        break;
      case RM_FREE:
        rep = HandleFree(&reserv, req.tr_num, req.node);
        break;
      default: assert(0);
    }

    PrintReservationData(&reserv);

    Reply(req_tid, &rep, sizeof(rep));
  }
}


int Reserve(tid_t rm_tid, int tr_num, track_node *s, int dist) {
  RMReq req;
  RMReply rep;
  req.type = RM_RESERVE;
  req.tr_num = tr_num;
  req.node = s;
  req.dist = dist;

  Send(rm_tid, &req, sizeof(req), &rep, sizeof(rep));
  return rep.ret;
}

int Free(tid_t rm_tid, int tr_num, track_node *s) {
  RMReq req;
  RMReply rep;
  req.type = RM_FREE;
  req.tr_num = tr_num;
  req.node = s;

  Send(rm_tid, &req, sizeof(req), &rep, sizeof(rep));
  return rep.ret;
}
