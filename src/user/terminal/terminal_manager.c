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
    // PutC(tx_tid, c);
    req.data = &c;
    Send(tman_tid, &req, sizeof(req), &rep, sizeof(rep));
  }
}

void wm_init(WManager *wm) {
  tdisp_init(&wm->td);
  wm->x = 3;
  wm->y = 1;
  tdisp_add_window(&wm->td, wm->x, wm->y, 30, 10, -1);
  wm->y += 11;
}

void wm_add_window(WManager *wm, tid_t tid) {
  tdisp_add_window(&wm->td, wm->x, wm->y, 30, 10, tid);
  wm->y += 11;
  tdisp_focus_window(&wm->td, 0);
}

void print_tdisp(tid_t tx_tid, TDisplay *td) {
  char c;
  while (td->buffer.size > 0) {
    tdisp_cb_pop(&td->buffer, &c);
    PutC(tx_tid, c);
  }
}

void TerminalManager() {
  int r;
  char *c;
  WManager wm;
  tid_t recv_tid, tx_tid;
  TManReq req;
  TManRep rep;

  r = RegisterAs(TERMINAL_MANAGER_ID);
  assert(r == 0);
  tx_tid = WhoIs(TERMINAL_UART_TX);
  assert(tx_tid > 0);

  Create(30, &TerminalInputHandler);

  wm_init(&wm);
  PutStr(tx_tid, "\033[2J", 5);
  PutStr(tx_tid, "\033[2J", 5);
  print_tdisp(tx_tid, &wm.td);

  while (true) {
    Receive(&recv_tid, &req, sizeof(req));
    c = req.data;

    switch (req.type) {
      case TERM_IN:
        switch (*c) {
          case 'q':
            assert(0);
            break;
          case '+':
            wm_add_window(&wm, -1);
            break;
          case '-':
            tdisp_delete_window(&wm.td);
            break;
          case '1':
            tdisp_focus_window(&wm.td, 0);
            break;
          case '2':
            tdisp_focus_window(&wm.td, 1);
            break;
          case '3':
            tdisp_focus_window(&wm.td, 2);
            break;
          default:
            tdisp_writec(&wm.td, *c);
            break;
        }

        Reply(recv_tid, &rep, sizeof(rep));
        break;
      case TERM_REG:
        wm_add_window(&wm, recv_tid);
        Reply(recv_tid, &rep, sizeof(rep));
        break;
      case TERM_OUT:
        tdisp_write_task(&wm.td, recv_tid, *c);
        Reply(recv_tid, &rep, sizeof(rep));
        break;
      default:
        KASSERT(0);
        break;
    }
    print_tdisp(tx_tid, &wm.td);
  }
}


void TMRegister(tid_t tm_tid) {
  TManReq req;
  TManReq rep;
  req.type = TERM_REG;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
}

void TMPutC(tid_t tm_tid, char c) {
  TManReq req;
  TManReq rep;
  req.type = TERM_OUT;
  req.data = &c;
  req.len  = 1;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
}


void TMPutStr(tid_t tm_tid, char *c, int len) {
  TManReq req;
  TManReq rep;
  req.type = TERM_OUT;
  req.data = c;
  req.len  = len;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
}
