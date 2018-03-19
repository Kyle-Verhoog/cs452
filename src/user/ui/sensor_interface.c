#include <user/ui/sensor_interface.h>

static tid_t tm_tid;

static void SensorSubscriber() {
  int r;
  tid_t rep_tid, par_tid;
  TrackRequest tr_req;
  TESEChange event;

  par_tid = MyParentTid();
  assert(par_tid > 0);
  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);

  tr_req.type = TRR_SUBSCRIBE;
  tr_req.data.type = TE_SE_CHANGE;

  while (true) {
    Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
    Send(par_tid, &event.num, sizeof(event.num), &r, sizeof(r));
  }
  Exit();
}

void SensorInterface() {
  tid_t sub_tid;
  int i, offset;
  char buf[256];
  rec_buffer rb;

  rec_buffer_init(&rb);

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  TMRegister(tm_tid, SENSOR_OFF_X, SENSOR_OFF_Y, SENSOR_WIDTH, SENSOR_HEIGHT);

  Create(11, &SensorSubscriber);

  int sen_num;
  while (true) {
    Receive(&sub_tid, &sen_num, sizeof(sen_num));
    Reply(sub_tid, &i, sizeof(i));
    rec_buffer_add(&rb, sen_num);

    offset = 0;
    offset += buf_pack_c(buf+offset, TERM_RESET);

    // Print from Latest Sensor
    for(i = 0; i < min(rb.num, 4); ++i) {
      sen_num = rec_buffer_get(&rb, i);
      offset += buf_pack_c(buf+offset, ' ');
      offset += buf_pack_c(buf+offset, sen_num/16 + 'A');
      offset += buf_pack_i32(buf+offset, sen_num%16 + 1);
      if ((sen_num%16)+1 < 10)
        offset += buf_pack_c(buf+offset, ' ');
    }

    TMPutStr(tm_tid, buf, offset);
  }

  Exit();
}
