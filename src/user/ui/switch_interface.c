#include <user/ui/switch_interface.h>

static void SwitchSubscriber() {
  int r;
  tid_t rep_tid, par_tid;
  TrackRequest tr_req;
  TESWChange event;

  par_tid = MyParentTid();
  assert(par_tid > 0);
  rep_tid = WhoIs(REPRESENTER_ID);
  assert(rep_tid > 0);

  tr_req.type = TRR_SUBSCRIBE;
  tr_req.data.type = TE_SW_CHANGE;

  while (true) {
    Send(rep_tid, &tr_req, sizeof(tr_req), &event, sizeof(event));
    Send(par_tid, &event, sizeof(event), &r, sizeof(r));
  }
  Exit();
}

void SwitchInterface() {
  tid_t sub_tid, tm_tid;
  int i, offset;
  TESWChange event;
  char buf[256];
  rec_buffer rb;
  char switches[SWITCH_SIZE];

  for (i = 0; i < SWITCH_SIZE; ++i)
    switches[i] = '\0';
  for (i = NORMAL_SWITCH_SIZE_LOW; i <= NORMAL_SWITCH_SIZE_HIGH; ++i)
    switches[i] = '?';
  for (i = SPECIAL_SWITCH_SIZE_LOW; i <= SPECIAL_SWITCH_SIZE_HIGH; ++i)
    switches[i] = '?';

  rec_buffer_init(&rb);

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid > 0);

  TMRegister(tm_tid, SWITCH_OFF_X, SWITCH_OFF_Y, SWITCH_WIDTH, SWITCH_HEIGHT);
  TMLogRegister(tm_tid);

  Create(11, &SwitchSubscriber);

  char c;
  while (true) {
    Receive(&sub_tid, &event, sizeof(event));
    switches[event.num] = event.newdir == 33 ? 'S' : 'C';
    Reply(sub_tid, &i, sizeof(i));

    offset = 0;
    offset += buf_pack_c(buf+offset, TERM_RESET);
    offset += buf_pack_f(buf+offset, "   sw      dir\n");
    offset += buf_pack_f(buf+offset, "  ┉┉┉┉┉┉┉┉┉┉┉┉┉\n");
    for (i = 0; i < SWITCH_SIZE; ++i) {
      c = switches[i];
      if (c != '\0') {
        offset += buf_pack_f(buf+offset, "   %d\t%c\n", i, c);
      }
    }
    TMPutStr(tm_tid, buf, offset);
  }

  Exit();
}
