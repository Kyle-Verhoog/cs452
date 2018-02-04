#include <user/clockserver.h>


int Delay(int tid, uint32_t ticks) {
  int cs_tid = WhoIs(CLOCKSERVER_ID);
}

int DelayUntil(int tid, int ticks) {

}

int Time(int tid) {
}

void ClockServer() {
  // init queue
  cs_queue csq;  
  csq_init(&csq);
  uint32_t ticks;
  ticks = 0;

  // init timer 3

  tid_t req_tid;
  CSReq req;

  while (true) {
    Receive(&req_tid, &req, sizeof(CSReq)); // TODO: AwaitEvent?

    switch (req.type) {
      case CSREQ_DELAY:
        KASSERT(0 && "TODO");
        break;
      case CSREQ_UNTIL:
        KASSERT(0 && "TODO");
        break;
      case CSREQ_TIME:
        KASSERT(0 && "TODO");
        break;
      case CSREQ_UPDATE:
        ticks++;
        KASSERT(0 && "TODO");
        break;
      default:
        KASSERT(0 && "CS: UNKNOWN REQUEST TYPE");
        break;
    }
  }

  Exit();
}
