#include <user/ioserver.h>

CIRCULAR_BUFFER_DEF(io_cb, volatile char, 512);


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
    // SANITY();
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
  notargs.type  = IO_RX;
  notargs.ie    = ie_base + IE_UART_RX_OFFSET;
  notargs.emask = RIEN_MASK;
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);

  tid_t req_tid, queued_tid;
  IOServerReq req;
  int rep;
  int volatile *data;
  char c;
  io_cb recv_buf;

  io_cb_init(&recv_buf);
  rep = 0;
  queued_tid = -1;
  data = (int *)(uart_base + UART_DATA_OFFSET);

  while (true) {
    // SANITY();
    Receive(&req_tid, &req, sizeof(req));
    // SANITY();
    switch (req.type) {
      case IO_GETC:
        // SANITY();
        if (recv_buf.size > 0) {
          r = io_cb_pop(&recv_buf, &c);
          assert(r == 0);
          Reply(req_tid, &c, sizeof(c));
        } else {
          if (queued_tid != -1)
            assert(queued_tid == req_tid && "detected multiple getc-ers");
          queued_tid = req_tid;
        }
        break;
      case IO_RT:
        assert(0);
        break;
      case IO_RX:
        c = *data;
        r = io_cb_push(&recv_buf, c);
        assert(r == 0 && "io buffer overflow");
        if (queued_tid > 0 && recv_buf.size > 0) {
          r = io_cb_pop(&recv_buf, &c);
          assert(r == 0);
          Reply(queued_tid, &c, sizeof(c));
          queued_tid = -1;
        }
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
  int r;
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
  r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);

  // if we are using UART1 (to the train), create a notifier for MI
  if (cts_en) {
    notargs.type  = IO_MI;
    notargs.ie    = ie_base + IE_UART_MI_OFFSET;
    notargs.emask = MSIEN_MASK; //0x8;
    r = CreateArgs(31, &IOServerNotifier, (void *)&notargs);
  }

  // SANITY();
  tid_t req_tid;
  IOServerReq req;
  int rep;
  int *data;
  char c;
  tid_t not_tid;
  bool tx_ready;
  int cts_count;
  io_cb tran_buf;

  io_cb_init(&tran_buf);
  data  = (int *)(uart_base + UART_DATA_OFFSET);
  not_tid  = -1;
  tx_ready = false;
  cts_count = 0;
  rep = 0;

  // while (true) {
  //   // SANITY();
  //   Receive(&req_tid, &req, sizeof(req));
  //   switch (req.type) {
  //     case IO_PUTC:
  //       // SANITY();
  //       assert(req.len == sizeof(char));
  //       r = io_cb_push(&tran_buf, req.msg[0]);
  //       rep_notif = true;
  //       Reply(req_tid, &rep, sizeof(rep));
  //       break;
  //     case IO_TX:
  //       // SANITY();
  //       not_tid = req_tid;

  //       while (tran_buf.size > 0 && !(*flags & TXFF_MASK)) {
  //         rep_notif = true;
  //         r = io_cb_pop(&tran_buf, &c);
  //         assert(r == 0 && "io buffer overflow");
  //         *data = c;
  //       }
  //       break;
  //     case IO_MI:
  //       PRINTF("MI\r\n");
  //       break;
  //     default:
  //       assert(0 && "INVALID INTERRUPT");
  //       break;
  //   }

  //   if (rep_notif && not_tid > -1) {
  //     // SANITY();
  //     Reply(not_tid, &rep, sizeof(rep));
  //     rep_notif = false;
  //   }
  // }

  bool tx_ready = 0;
  while(true){
    Receive(&req_tid, &req, sizeof(req));
    switch(req.type){
      case IO_PUTC:
        // SANITY();
        c = req.msg;
        r = io_cb_push(&tran_buf, c);
        assert(req.len == sizeof(char));

        if (tx_ready && (!cts_en || (cts_en && cts_count > 1))) {
          r = io_cb_pop(&tran_buf, &c);
          assert(r == 0);
          *data = c;
          tx_ready = false;
          cts_count = 0;
          assert(not_tid > 0);
          Reply(not_tid, &rep, sizeof(rep));
        }

        Reply(req_tid, &rep, sizeof(rep));
        if(!tx_ready) break;
        //fall through if ready
      case IO_TX:
        // SANITY();
        not_tid = req_tid;
        tx_ready = true;

        if (tran_buf.size > 0 && (!cts_en || (cts_en && cts_count > 1))) {
          r = io_cb_pop(&tran_buf, &c);
          assert(r == 0);
          *data = c;
          tx_ready = false;
          cts_count = 0;
          Reply(req_tid, &rep, sizeof(rep));
        }
        break;
      case IO_MI:
        PRINTF("MI\r\n");
        cts_count++;

        if (tx_ready && tran_buf.size > 0 && cts_count > 1) {
          r = io_cb_pop(&tran_buf, &c);
          assert(r == 0);
          *data = c;
          tx_ready = false;
          cts_count = 0;
          assert(not_tid > 0);
          Reply(not_tid, &rep, sizeof(rep));
        }

        Reply(req_tid, &rep, sizeof(rep));
        break;
      default:
        assert(0 && "INVALID INTERRUPT");
        break;
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
  r = CreateArgs(31, &IOServerRX, (void *)&arg);

  // Create the TX server
  arg.ns_id = IOSERVER_UART2_TX_ID;
  r = CreateArgs(31, &IOServerTX, (void *)&arg);


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

  // Enable fifo
  *(int *)(UART1_BASE + UART_LCRH_OFFSET) = 0;
  *(int *)(UART1_BASE + UART_LCRH_OFFSET) |= STP2_MASK;
  *(int *)(UART1_BASE + UART_LCRH_OFFSET) &= ~FEN_MASK;

  // Set speed to 2400 bps
  *(int *)(UART1_BASE + UART_LCRM_OFFSET) = 0x0;
  *(int *)(UART1_BASE + UART_LCRL_OFFSET) = 0xbf; // (7.3728MHz / (16*baud)) - 1

  // Create the RX server
  arg.ns_id = IOSERVER_UART1_RX_ID;
  r = CreateArgs(31, &IOServerRX, (void *)&arg);

  // Create the TX server
  arg.ns_id = IOSERVER_UART1_TX_ID;
  r = CreateArgs(31, &IOServerTX, (void *)&arg);

  // Enable UART
  *(int *)(UART1_BASE + UART_CTRL_OFFSET) = UARTEN_MASK | RIEN_MASK | TIEN_MASK | MSIEN_MASK;
  Exit();
}

/**
 * NOTE: for the IO servers to be initialized properly, this task must be given
 *       priority strictly less than that given to IOServerRX and IOServerTX
 */
// void IOServerUART1() {
//   IOServerArgs arg;
//   int r;
// 
//   // Enable the UART and interrupts
//   // Enable fifo
//   *(int *)(UART1_BASE + UART_LCRH_OFFSET) |= STP2_MASK;
//   *(int *)(UART1_BASE + UART_LCRH_OFFSET) &= ~FEN_MASK;
// 
//   // Set speed to 2400 bps
//   *(int *)(UART1_BASE + UART_LCRM_OFFSET) = 0x0;
//   *(int *)(UART1_BASE + UART_LCRL_OFFSET) = 0xbf;
// 
//   // these don't change for initializing the servers
//   arg.uart_base = UART1_BASE;
//   arg.ie_base   = IE_UART1_BASE;
//   arg.cts_en    = true;
// 
//   // Create the RX server
//   arg.ns_id = IOSERVER_UART1_RX_ID;
//   r = CreateArgs(31, &IOServerRX, (void *)&arg);
// 
//   // Create the TX server
//   arg.ns_id = IOSERVER_UART1_TX_ID;
//   r = CreateArgs(31, &IOServerTX, (void *)&arg);
// 
//   // Enable UART
//   *(int *)(UART1_BASE + UART_CTRL_OFFSET) = UARTEN_MASK | RIEN_MASK | TIEN_MASK | MSIEN_MASK;
//   Exit();
// }


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
  int rep;

  req.type = IO_PUTC;
  req.msg = c;
  req.len = 1;

  // SANITY();
  r = Send(ios_tid, &req, sizeof(req), &rep, sizeof(rep));
  // SANITY();
  assert(r == 0);
  return 0;
}
