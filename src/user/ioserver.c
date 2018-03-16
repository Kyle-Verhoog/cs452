#include <user/ioserver.h>

void IOServerNotifier(void *args) {
  int r, rep, emask;
  IONotifierArgs *myargs;
  int volatile *ictrl;
  tid_t server_tid;
  IOServerReq req;
  InterruptEvent ie;

  myargs = (IONotifierArgs *)args;
  ictrl    = (int *)(myargs->uart + UART_CTRL_OFFSET);
  ie       = myargs->ie;
  emask    = myargs->emask;
  req.type = myargs->type;
  server_tid = myargs->tid;

  while (true) {
    *ictrl |= emask;
    AwaitEvent(ie);
    r = Send(server_tid, &req, sizeof(req), &rep, sizeof(rep));
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
  notargs.type  = IO_RX;
  notargs.ie    = ie_base + IE_UART_RX_OFFSET;
  notargs.emask = RIEN_MASK;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs, sizeof(notargs));

  tid_t req_tid, queued_tid;
  IOServerReq req;
  int rep;
  int volatile *data;
  iobuf_item iobi;
  io_cb recv_buf;

  io_cb_init(&recv_buf);
  rep = 0;
  queued_tid = -1;
  data = (int *)(uart_base + UART_DATA_OFFSET);

  while (true) {
    Receive(&req_tid, &req, sizeof(req));

    switch (req.type) {
      case IO_GETC:
        if (recv_buf.size > 0) {
          r = io_cb_pop(&recv_buf, &iobi);
          assert(r == 0);
          Reply(req_tid, &iobi.c, sizeof(char));
        } else {
          if (queued_tid != -1)
            assert(queued_tid == req_tid && "detected multiple getc-ers");
          queued_tid = req_tid;
        }

        break;

      case IO_RX:
        iobi.c = *data;
        r = io_cb_push(&recv_buf, iobi);
        assert(r == 0 && "io buffer overflow");

        if (queued_tid > 0 && recv_buf.size > 0) {
          r = io_cb_pop(&recv_buf, &iobi);
          assert(r == 0);
          Reply(queued_tid, &iobi.c, sizeof(char));
          queued_tid = -1;
        }

        Reply(req_tid, &rep, sizeof(rep));
        break;

      case IO_FLUSH:
        io_cb_init(&recv_buf);
        Reply(req_tid, &rep, sizeof(rep));
        break;

      default:
        assert(0 && "INVALID RT INTERRUPT");
        break;
    }
  }
}

void IOServerTX(void *args) {
  IOServerArgs *arg;
  tid_t mytid;
  int r, i;
  int uart_base, ns_id;
  bool cts_en;
  IONotifierArgs notargs;
  InterruptEvent ie_base;

  arg = (IOServerArgs *)args;
  uart_base = arg->uart_base;
  ns_id     = arg->ns_id;
  ie_base   = arg->ie_base;
  cts_en    = arg->cts_en;

  mytid = MyTid();

  r = RegisterAs(ns_id);
  assert(r == 0);

  notargs.uart  = uart_base;
  notargs.tid   = mytid;
  notargs.type  = IO_TX;
  notargs.ie    = ie_base + IE_UART_TX_OFFSET;
  notargs.emask = TIEN_MASK; //0x20;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs, sizeof(notargs));

  // if we are using UART1 (to the train), create a notifier for MI
  if (cts_en) {
    notargs.type  = IO_MI;
    notargs.ie    = ie_base + IE_UART_MI_OFFSET;
    notargs.emask = MSIEN_MASK; //0x8;
    r = CreateArgs(31, &IOServerNotifier, (void *)&notargs, sizeof(notargs));
  }

  tid_t req_tid;
  IOServerReq req;
  int rep;
  int *data;
  char *str;
  tid_t txnot_tid;
  bool tx_ready;
  int cts_count;
  iobuf_item iobi;
  iobuf_item iobr;
  io_cb tran_buf;

  io_cb_init(&tran_buf);
  iobi.btid = -1;
  iobr.btid = -1;
  txnot_tid = -1;
  tx_ready  = true;
  cts_count = 2;
  rep  = 0;
  data = (int *)(uart_base + UART_DATA_OFFSET);

  while (true) {
    Receive(&req_tid, &req, sizeof(req));
    switch (req.type) {
      case IO_PUTC:
        iobi.c = *(req.msg);
        iobi.btid = -1;
        r = io_cb_push(&tran_buf, iobi);
        assert(r == 0);
        assert(req.len == sizeof(char));

        if (tx_ready && (!cts_en || (cts_en && cts_count > 1)))
          goto TX_CHAR;
        break;

      case IO_BLPUTC:
        iobi.c = *(req.msg);
        iobi.btid = req_tid;
        r = io_cb_push(&tran_buf, iobi);
        assert(r == 0);
        assert(req.len == sizeof(char));

        if (tx_ready && (!cts_en || (cts_en && cts_count > 1)))
          goto TX_CHAR;

        break;

      case IO_PUTSTR:
        str = req.msg;
        iobi.btid = -1;

        for (i = 0; i < req.len; ++i) {
          iobi.c = str[i];
          r = io_cb_push(&tran_buf, iobi);
          assert(r == 0);
        }

        if (tx_ready && (!cts_en || (cts_en && cts_count > 1)))
          goto TX_CHAR;

        break;

      case IO_TX:
        txnot_tid = req_tid;
        tx_ready  = true;

        if (tran_buf.size > 0 && (!cts_en || (cts_en && cts_count > 1)))
          goto TX_CHAR;

        break;

      case IO_MI:
        cts_count++;

        if (tx_ready && tran_buf.size > 0 && cts_count > 1)
          goto TX_CHAR;

        break;

      case IO_FLUSH:
        io_cb_init(&tran_buf);
        Reply(req_tid, &rep, sizeof(rep));
        break;

      default:
        assert(0 && "INVALID INTERRUPT");
        break;
    }

    // if we got here, then skip the transmit logic and reply to the sender if
    // the sender is not the tx-er or the blputc-er
    if (txnot_tid != req_tid && iobi.btid != req_tid)
      Reply(req_tid, &rep, sizeof(rep));
    continue;

TX_CHAR:
    r = io_cb_pop(&tran_buf, &iobr);
    assert(r == 0);
    *data = iobr.c;
    tx_ready  = false;
    cts_count = 0;

    assert(txnot_tid > 0);

    // in all cases, if we've gotten here we want to unblock the transmitter
    // notifier
    Reply(txnot_tid, &rep, sizeof(rep));

    // if the current requestor is not the tx notifier, or a blputc task then
    // reply back immediately
    if (txnot_tid != req_tid && iobi.btid != req_tid)
      Reply(req_tid, &rep, sizeof(rep));

    // if btid is a valid value, then reply to the blocked putc
    if (iobr.btid != -1)
      Reply(iobr.btid, &rep, sizeof(rep));
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
  arg.cts_en    = false;

  // TODO: arg.flags
  // Enable the UART and interrupts
  // Enable fifo
  *(int *)(UART2_BASE + UART_LCRH_OFFSET) &= ~FEN_MASK;

  // Set speed to 115200 bps
  *(int *)(UART2_BASE + UART_LCRM_OFFSET) = 0x0;
  *(int *)(UART2_BASE + UART_LCRL_OFFSET) = 0x3;

  // Create the RX server
  arg.ns_id = IOSERVER_UART2_RX_ID;
  r = CreateArgs(31, &IOServerRX, (void *)&arg, sizeof(arg));

  // Create the TX server
  arg.ns_id = IOSERVER_UART2_TX_ID;
  r = CreateArgs(31, &IOServerTX, (void *)&arg, sizeof(arg));


  // Enable UART
  *(int *)(UART2_BASE + UART_CTRL_OFFSET) = UARTEN_MASK | RIEN_MASK | TIEN_MASK;

  Exit();
}

void IOServerUART1() {
  IOServerArgs arg;
  int r;

  // these don't change for initializing the servers
  arg.uart_base = UART1_BASE;
  arg.ie_base   = IE_UART1_BASE;
  arg.cts_en    = true;

  // Set speed to 2400 bps
  *(int *)(UART1_BASE + UART_LCRM_OFFSET) = 0x0;
  *(int *)(UART1_BASE + UART_LCRL_OFFSET) = 0xbf; // (7.3728MHz / (16*baud)) - 1

  *(int *)(UART1_BASE + UART_LCRH_OFFSET) |= STP2_MASK;
  *(int *)(UART1_BASE + UART_LCRH_OFFSET) &= ~FEN_MASK;


  // Create the RX server
  arg.ns_id = IOSERVER_UART1_RX_ID;
  r = CreateArgs(31, &IOServerRX, (void *)&arg, sizeof(arg));

  // Create the TX server
  arg.ns_id = IOSERVER_UART1_TX_ID;
  r = CreateArgs(31, &IOServerTX, (void *)&arg, sizeof(arg));

  // Enable UART
  *(int *)(UART1_BASE + UART_CTRL_OFFSET) = UARTEN_MASK | RIEN_MASK | TIEN_MASK | MSIEN_MASK;
  Exit();
}


char GetC(tid_t ios_tid) {
  int r;
  assert(ios_tid > 0);

  IOServerReq req;
  char rep;

  req.type = IO_GETC;
  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  assert(r == 0);
  return rep;
}


int PutC(tid_t ios_tid, char c) {
  int r;
  assert(ios_tid > 0);

  IOServerReq req;
  int rep;

  req.type = IO_PUTC;
  req.msg = &c;
  req.len = 1;

  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  assert(r == 0);
  return 0;
}

int BLPutC(tid_t ios_tid, char c) {
  int r;
  assert(ios_tid > 0);

  IOServerReq req;
  int rep;

  req.type = IO_BLPUTC;
  req.msg = &c;
  req.len = 1;

  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  assert(r == 0);
  return 0;
}

int PutStr(tid_t ios_tid, char *c, int len) {
  int r;
  assert(ios_tid > 0);

  IOServerReq req;
  int rep;

  req.type = IO_PUTSTR;
  req.msg = c;
  req.len = len;

  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  assert(r == 0);
  return 0;
}

int FlushIO(tid_t ios_tid){
  int r;
  assert(ios_tid > 0);

  IOServerReq req;
  int rep;

  req.type = IO_FLUSH;

  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  assert(r == 0);
  return 0;
}
