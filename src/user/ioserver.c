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

void IOServerUART2() {
  tid_t mytid;
  int r;
  IONotifierArgs notargs;

  mytid = MyTid();

  r = RegisterAs(IOSERVER_UART2_ID);
  assert(r == 0);

  notargs.uart = UART2_BASE;
  notargs.tid  = mytid;
  
  // notargs.inter = IE_UART2_RX;
  // r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  // notargs.inter = IE_UART2_TX;
  // notargs.emask = 0x20;
  // r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  notargs.inter = IE_UART2_RT;
  notargs.emask = 0x40;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  // notargs.inter = IE_UART2_MI;
  // r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);

  // TODO: configuration, set speed, fifo, etc
  //*(int *)(UART2_BASE + UART_CTRL_OFFSET) = 0x59;
  *(int *)(UART2_BASE + UART_CTRL_OFFSET) = 0x41;
  
  // SANITY();
  tid_t req_tid, queued_tid;
  int req;
  int rep = 0;
  int volatile *data, *flags;
  char c;
  io_cb recv_buf;

  io_cb_init(&recv_buf);
  queued_tid = -1;
  while (true) {
    // SANITY();
    Receive(&req_tid, &req, sizeof(req));
    switch (req) {
      case IO_GETC:
        if (recv_buf.size > 0) {
          r = io_cb_pop(&recv_buf, &c);
          assert(r == 0);
          Reply(req_tid, &c, sizeof(c));
        } else {
          queued_tid = req_tid;
        }
        continue;
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
          r = io_cb_push(&recv_buf, c);
          assert(r == 0 && "io buffer overflow");
        }

        if (queued_tid > 0 && recv_buf.size > 0) {
          r = io_cb_pop(&recv_buf, &c);
          assert(r == 0);
          Reply(queued_tid, &c, sizeof(c));
          queued_tid = -1;
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


char GetC(tid_t ios_tid) {
  int r;
  assert(ios_tid > 0);

  int req;
  char rep;

  req = IO_GETC;
  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  assert(r == 0);
  return rep;
}
