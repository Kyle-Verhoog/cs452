#include <user/terminal/terminal_manager.h>

CIRCULAR_BUFFER_DEF(term_cb, char, TERMINAL_BUFFER_SIZE);

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

void wm_init(WManager *wm) {
  tdisp_init(&wm->td);
}

void wm_add_window(WManager *wm, tid_t tid, char *conf) {
  tdisp_add_window(&wm->td, (int)conf[0], (int)conf[1], (int)conf[2], (int)conf[3], tid);
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
  char ch;
  WManager wm;
  tid_t recv_tid, tx_tid;
  TManReq req;
  TManRep rep;
  term_cb buf;
  bool  sh_rdy;
  tid_t sh_tid;

  sh_rdy = false;
  sh_tid = -1;
  term_cb_init(&buf);
  r = RegisterAs(TERMINAL_MANAGER_ID);
  assert(r == 0);
  tx_tid = WhoIs(TERMINAL_UART_TX);
  assert(tx_tid > 0);

  Create(30, &TerminalInputHandler);

  wm_init(&wm);
  PutStr(tx_tid, "\033[2J", 5);
  PutStr(tx_tid, "\033[2J", 5);
  print_tdisp(tx_tid, &wm.td);

  Create(30, &Shell);

  while (true) {
    Receive(&recv_tid, &req, sizeof(req));
    c = req.data;

    switch (req.type) {
      case TERM_GET:
        sh_tid = recv_tid;
        if (buf.size > 0) {
          r = term_cb_pop(&buf, &ch);
          assert(r == 0);
          rep.data = ch;
          Reply(recv_tid, &rep, sizeof(rep));
          sh_rdy = false;
        } else {
          sh_rdy = true;
        }
        break;
      case TERM_IN:
        switch (*c) {
          case 27:
            // assert(0);
            Halt();
            break;
          case '+':
            wm_add_window(&wm, -1, 0);
            break;
          case 127:
            tdisp_delete_window(&wm.td);
            break;
          case '.':
            tdisp_focus_window(&wm.td, 0);
            break;
          default:
            break;
        }

        if (sh_rdy) {
          rep.data = *c;
          Reply(sh_tid, &rep, sizeof(rep));
        }
        else {
          r = term_cb_push(&buf, *c);
          assert(r == 0);
        }

        Reply(recv_tid, &rep, sizeof(rep));
        break;
      case TERM_REG:
        wm_add_window(&wm, recv_tid, req.data);
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

void TMRegister(tid_t tm_tid, char offsetx, char offsety, char width, char height) {
  TManReq req;
  TManRep rep;
  char config[4];
  req.type = TERM_REG;
  config[0] = offsetx;
  config[1] = offsety;
  config[2] = width;
  config[3] = height;
  req.data = config;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
}

void TMPutC(tid_t tm_tid, char c) {
  TManReq req;
  TManRep rep;
  req.type = TERM_OUT;
  req.data = &c;
  req.len  = 1;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
}

void TMPutStr(tid_t tm_tid, char *c, int len) {
  TManReq req;
  TManRep rep;
  req.type = TERM_OUT;
  req.data = c;
  req.len  = len;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
}

char TMGetC(tid_t tm_tid) {
  TManReq req;
  TManRep rep;
  req.type = TERM_GET;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
  return rep.data;
}
