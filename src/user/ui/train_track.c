#include <user/ui/train_track.h>

void TrainTrackInterface() {
  // const char *empty_node = "□";
  // const char *occup_node = "■";
  // const char *train_right = "▶";
  // const char *train_left  = "◀";
  // const char *train_up    = "▲";
  // const char *train_down  = "▼";
  // const char *rect = "▬";
  const char *track = " ═══════╦════╦════════▶═════════════════╗\n"
                      " ═══════╝ ╔══╝═════╦════╦════════════   ║\n"
                      "  ═════╝  ║        ╚╗  ╔╝               ▲\n"
                      "          ║         ║  ║                ║\n"
                      "          ║         ╚╗╔╝                ║\n"
                      "          ║         ╚  ╝                ║\n"
                      "          ║  ═══════╝\n";
  // const char *track = "   ╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶╶\n"
  //                     "  ╱       ╲╱ ▲              ╲   \n"
  //                     "          ╱╲                 \          ║\n"
  //                     "                   ║\n"
  //                     "            ═══════╝\n";

  tid_t my_tid, cs_tid, tm_tid;

  my_tid = MyTid();
  assert(my_tid >= 0);
  cs_tid = WhoIs(CLOCKSERVER_ID);
  assert(cs_tid >= 0);
  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);
  TMRegister(tm_tid, TRACK_OFF_X, TRACK_OFF_Y, TRACK_WIDTH, TRACK_HEIGHT);

  // TMPutStr(tm_tid, occup_node, strlen(occup_node));
  TMPutStr(tm_tid, track, strlen(track));
  while (true) {
    // TMPutStrf(tm_tid, "→\n");
    // TMLogStrf(tm_tid, "→\n");
    // PutStr(tx_tid, "→\n", strlen("→\n"));
    // TMPutStr(tm_tid, "→", strlen("→"));
    // TMPutC(tm_tid, empty_node[0]);
    // TMPutC(tm_tid, empty_node[1]);
    // TMPutC(tm_tid, empty_node[2]);
    // TMPutC(tm_tid, ' ');
    DelayCS(cs_tid, 50);
  }

  Exit();
}
