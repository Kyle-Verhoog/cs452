#include <user/terminal/terminal_manager.h>

void TerminalInputHandler() {
  tid_t tman_tid, inpt_tid;
  char c;
  TManReq req;
  TManRep rep;

  req.type = TERM_IN;
  req.len = 1;
  tman_tid = WhoIs(TERMINAL_MANAGER_ID);
  inpt_tid = WhoIs(TERMINAL_UART_RX);

  while (true) {
    c = GetC(inpt_tid);
    req.data = &c;
    Send(tman_tid, &req, sizeof(req), &rep, sizeof(rep));
  }
}

void tdisp_print(TDisplay *td, tid_t tx_tid) {
  char c;
  while (td->buffer.size > 0) {
    tdisp_cb_pop(&td->buffer, &c);
    PutC(tx_tid, c);
  }
}

void TerminalManager() {
  int r;
  TDisplay term_disp;
  tid_t recv_tid, tx_tid;
  TManReq req;
  TManRep rep;

  tdisp_init(&term_disp);
  r = RegisterAs(TERMINAL_MANAGER_ID);
  assert(r == 0);
  tx_tid = WhoIs(TERMINAL_UART_TX);
  assert(tx_tid > 0);

  Create(30, &TerminalInputHandler);

  PutStr(tx_tid, "\033[2J", 5);
  PutStr(tx_tid, "\033[2J", 5);

  while (true) {
    Receive(&recv_tid, &req, sizeof(req));

    switch (req.type) {
      case TERM_IN:
        PutC(tx_tid, *(req.data));
        // PutStr(tx_tid, req.data, 1);
        // PutStr(tx_tid, "\033[2J", 5);
        Reply(recv_tid, &rep, sizeof(rep));
        break;
      default:
        KASSERT(0);
        break;
    }
  }
}
