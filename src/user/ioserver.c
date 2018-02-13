#include <user/ioserver.h>

CIRCULAR_BUFFER_DEF(io_cb, char, 512);

void IOServerNotifier(void *args) {
  int r;
  IONotifierArgs *myargs;
  myargs = (IONotifierArgs *)args;
  int req = myargs->inter;
  int rep;
  int volatile *ictrl = (int *)(myargs->uart + UART_CTRL_OFFSET);
  int emask = myargs->emask;

  // SANITY();
  while (true) {
    *ictrl |= emask;
    AwaitEvent(req);
    // SANITY();
    r = Send(myargs->tid, &req, sizeof(req), &rep, sizeof(rep));
    // SANITY();
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

void IOServerUART2() {
  tid_t mytid;
  int r;
  IONotifierArgs notargs;

  mytid = MyTid();

  notargs.uart = UART2_BASE;
  notargs.tid  = mytid;
  
  notargs.inter = IE_UART2_RX;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  notargs.inter = IE_UART2_TX;
  notargs.emask = 0x20;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  notargs.inter = IE_UART2_RT;
  notargs.emask = 0x40;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  notargs.inter = IE_UART2_MI;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);

  *(int *)(UART2_BASE + UART_CTRL_OFFSET) = 0x59;
  
  // SANITY();
  tid_t req_tid;
  int req;
  int rep = 0;
  int volatile *data, *flags;
  char c;
  io_cb recv_buf;
  while (true) {
    // SANITY();
    Receive(&req_tid, &req, sizeof(req));
    switch (req) {
      case IE_UART2_RX:
        PRINTF("RX\r\n");
        break;
      case IE_UART2_TX:
        PRINTF("TX\r\n");
        break;
      case IE_UART2_RT:
        data  = (int *)(UART2_BASE + UART_DATA_OFFSET);
        flags = (int *)(UART2_BASE + UART_FLAG_OFFSET);

        while (!(*flags & RXFE_MASK)) {
          c = *data;
          io_cb_push(&recv_buf, c);
          PRINTF("RT %c\r\n", c);
        }
        break;
      case IE_UART2_MI:
        PRINTF("MI\r\n");
        break;
      default:
        assert(0 && "INVALID INTERRUPT");
        break;
    }
    Reply(req_tid, &rep, sizeof(rep));
  }
}
