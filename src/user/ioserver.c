#include <user/ioserver.h>


void IOServerNotifier(void *args) {
  int r;
  IONotifierArgs *myargs;
  myargs = (IONotifierArgs *)args;
  int req = myargs->inter;
  int rep;

  while (true) {
    AwaitEvent(req);
    r = Send(myargs->tid, &req, sizeof(req), &rep, sizeof(rep));
    assert(r == 0);
    assert(rep == 0);
  }
}

/*
void IOServerRX(void *args) {
  int r;
  tid_t mytid, req_tid;
  IOServerArgs *myargs;
  IONotifierArgs notargs;
  myargs = (IOServerArgs *)args;

  mytid = MyTid();
  assert(IS_VALID_ID(mytid));

  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  assert(r == 0);

  while (true) {
    assert(r == 0);

  }
}

void IOServerTX(void *args) {
  int r;
  tid_t mytid;
  IOServerArgs *myargs;
  IONotifierArgs notargs;
  myargs = (IOServerArgs *)args;

  mytid = MyTid();
  assert(IS_VALID_ID(mytid));
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
}
*/

void IOServerNotifiersTest() {
  tid_t mytid;
  int r;
  IONotifierArgs notargs;

  mytid = MyTid();

  notargs.uart  = UART2_BASE;
  notargs.tid   = mytid;
  
  notargs.inter = IE_UART2_RX;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  notargs.inter = IE_UART2_TX;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  notargs.inter = IE_UART2_RT;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  notargs.inter = IE_UART2_MI;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);

  tid_t req_tid;
  int req;
  int rep = 0;
  while (true) {
    Receive(&req_tid, &req, sizeof(req));
    switch (req) {
      case IE_UART2_RX:
        assert(0 && "RECEIVE");
        break;
      case IE_UART2_TX:
        assert(0 && "TRANSMIT");
        break;
      case IE_UART2_RT:
        assert(0 && "RECEIVE TIMEOUT");
        break;
        break;
      case IE_UART2_MI:
        assert(0 && "MODEM");
        break;
      default:
        assert(0 && "INVALID INTERRUPT");
        break;
    }
    Reply(req_tid, &rep, sizeof(rep));
  }
}
