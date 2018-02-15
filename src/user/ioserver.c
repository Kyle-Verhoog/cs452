#include <user/ioserver.h>

CIRCULAR_BUFFER_DEF(io_cb, char, 512);

void IOServerNotifier(void *args) {
  // SANITY();
  int r, rep, emask;
  IONotifierArgs *myargs;
  int volatile *ictrl;
  tid_t server_tid;
  IOServerReq req;
  InterruptEvent ie;

  // SANITY();

  myargs = (IONotifierArgs *)args;
  ictrl    = (int *)(myargs->uart + UART_CTRL_OFFSET);
  ie       = myargs->ie;
  emask    = myargs->emask;
  req.type = myargs->type;
  server_tid = myargs->tid;

  while (true) {
    *ictrl |= emask;
    AwaitEvent(ie);
    // SANITY();
    r = Send(server_tid, &req, sizeof(req), &rep, sizeof(rep));
    // SANITY();
    assert(r == 0);
    assert(rep == 0);
  }
}

void IOServerRX(void *args) {
  IOServerArgs *arg;
  tid_t mytid;
  int r;
  int uart_base, ns_id;
  IONotifierArgs notargs;
  InterruptEvent ie_base;

  arg = (IOServerArgs *)args;
  uart_base = arg->uart_base;
  ns_id     = arg->ns_id;
  ie_base   = arg->ie_base;

  mytid = MyTid();

  r = RegisterAs(ns_id);
  assert(r == 0);

  notargs.uart  = uart_base;
  notargs.tid   = mytid;
  notargs.type  = IO_RT;
  notargs.ie    = ie_base + IE_UART_RT_OFFSET;
  notargs.emask = RTIEN_MASK; // 0x40;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);

  tid_t req_tid, queued_tid;
  IOServerReq req;
  int rep = 0;
  int volatile *data, *flags;
  char c;
  io_cb recv_buf;

  io_cb_init(&recv_buf);
  queued_tid = -1;
  data  = (int *)(UART2_BASE + UART_DATA_OFFSET);
  flags = (int *)(UART2_BASE + UART_FLAG_OFFSET);

  while (true) {
    // SANITY();
    Receive(&req_tid, &req, sizeof(req));
    switch (req.type) {
      case IO_GETC:
        // SANITY();
        if (recv_buf.size > 0) {
          r = io_cb_pop(&recv_buf, &c);
          assert(r == 0);
          Reply(req_tid, &c, sizeof(c));
        } else {
          queued_tid = req_tid;
        }
        continue;
      case IO_RT:
        // SANITY();
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
      case IO_RX:
        assert(0 && "TODO");
        break;
      default:
        assert(0 && "INVALID RT INTERRUPT");
        break;
    }
    Reply(req_tid, &rep, sizeof(rep));
  }
}

void IOServerTX(void *args) {
  IOServerArgs *arg;
  tid_t mytid;
  int r;
  int uart_base, ns_id;
  IONotifierArgs notargs;
  InterruptEvent ie_base;

  arg = (IOServerArgs *)args;
  uart_base = arg->uart_base;
  ns_id     = arg->ns_id;
  ie_base   = arg->ie_base;

  mytid = MyTid();

  r = RegisterAs(ns_id);
  assert(r == 0);

  notargs.uart  = uart_base;
  notargs.tid   = mytid;
  notargs.type  = IO_TX;
  notargs.ie    = ie_base + IE_UART_TX_OFFSET;
  notargs.emask = TIEN_MASK; //0x20;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  // notargs.type  = IO_MI;
  // notargs.ie    = ie_base + IE_UART_MI_OFFSET;
  // notargs.emask =
  // r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);

  // SANITY();
  tid_t req_tid;
  IOServerReq req;
  int rep;
  int volatile *data, *flags;
  char c;
  bool rep_notif;
  tid_t not_tid;
  io_cb tran_buf;

  io_cb_init(&tran_buf);
  data  = (int *)(uart_base + UART_DATA_OFFSET);
  flags = (int *)(uart_base + UART_FLAG_OFFSET);
  rep_notif = false;
  not_tid = -1;
  rep = 0;

  while (true) {
    // SANITY();
    Receive(&req_tid, &req, sizeof(req));
    switch (req.type) {
      case IO_PUTC:
        // SANITY();
        assert(req.len == sizeof(char));
        r = io_cb_push(&tran_buf, req.msg[0]);
        rep_notif = true;
        // while (!(*flags & TXFF_MASK) && tran_buf.size > 0) {
        //   r = io_cb_pop(&tran_buf, &c);
        //   assert(r == 0 && "io buffer overflow");
        //   *data = c;
        // }
        Reply(req_tid, &rep, sizeof(rep));
        break;
      case IO_TX:
        // SANITY();
        not_tid = req_tid;

        while (tran_buf.size > 0 && !(*flags & TXFF_MASK)) {
          rep_notif = true;
          r = io_cb_pop(&tran_buf, &c);
          PRINTF("%c\r\n", c);
          assert(r == 0 && "io buffer overflow");
          *data = c;
        }
        break;
      case IO_MI:
        PRINTF("MI\r\n");
        break;
      default:
        assert(0 && "INVALID INTERRUPT");
        break;
    }

    if (rep_notif && not_tid > -1) {
      // SANITY();
      Reply(not_tid, &rep, sizeof(rep));
      rep_notif = false;
    }
  }
}


/**
 * NOTE: for the IO servers to be initialized properly, this task must be given
 *       priority strictly less than that given to IOServerRX and IOServerTX
 */
void IOServerUART2() {
  IOServerArgs arg;
  int r;

  // these don't change for initializing the servers
  arg.uart_base = UART2_BASE;
  arg.ie_base   = IE_UART2_BASE;

  // Create the RX server
  arg.ns_id = IOSERVER_UART2_RX_ID;
  r = CreateArgs(31, &IOServerRX, (void *)&arg);

  // Create the TX server
  arg.ns_id = IOSERVER_UART2_TX_ID;
  r = CreateArgs(31, &IOServerTX, (void *)&arg);

  // TODO: arg.flags
  // Enable the UART and interrupts
  *(int *)(UART2_BASE + UART_CTRL_OFFSET) = 0x61;
  // *(int *)(UART2_BASE + UART_CTRL_OFFSET) = UARTEN_MASK | RTIEN_MASK | TIEN_MASK;
  Exit();
}


char GetC(tid_t ios_tid) {
  int r;
  assert(ios_tid > 0);

  IOServerReq req;
  char rep;

  req.type = IO_GETC;
  // SANITY();
  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  // SANITY();
  assert(r == 0);
  return rep;
}

int PutC(tid_t ios_tid, char c) {
  int r;
  assert(ios_tid > 0);

  IOServerReq req;
  char rep;

  req.type = IO_PUTC;
  req.msg[0] = c;
  req.len = 1;

  // SANITY();
  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  // SANITY();
  assert(r == 0);
  return 0;
}
