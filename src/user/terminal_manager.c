#include <user/terminal_manager.h>

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
  char buf[20];
  int len;

  len = 0;
  while (td->buffer.size > 0) {
    tdisp_cb_pop(&td->buffer, &c);
    buf[len++] = c;
    if (len == 19) {
      PutStr(tx_tid, buf, len);
      len = 0;
    }
  }

  if (len > 0)
    PutStr(tx_tid, buf, len);
}

void TerminalManager() {
  int r, i;
  char *c;
  char ch;
  char tid_buf[10];
  WManager wm;
  tid_t recv_tid, tx_tid;
  TManReq req;
  TManRep rep;
  term_cb buf;
  bool  sh_rdy;
  tid_t sh_tid, log_tid;
  int log_line;

  sh_rdy = false;
  sh_tid = -1;
  log_tid = -1;
  log_line = 0;
  term_cb_init(&buf);
  r = RegisterAs(TERMINAL_MANAGER_ID);
  assert(r == 0);
  tx_tid = WhoIs(TERMINAL_UART_TX);
  assert(tx_tid > 0);

  Create(30, &TerminalInputHandler);

  wm_init(&wm);
  char *init = "\033[30m\033[47m\033[2J\033[1m\033[?25l";
  PutStr(tx_tid, init, strlen(init));
  PutStr(tx_tid, init, strlen(init));
  print_tdisp(tx_tid, &wm.td);

  int sh_args[4];
  sh_args[0] = SH_OFFSET_X;
  sh_args[1] = SH_OFFSET_Y;
  sh_args[2] = SH_WIDTH;
  sh_args[3] = SH_HEIGHT;
  CreateArgs(29, &Shell, (void *)sh_args);

  while (true) {
    Receive(&recv_tid, &req, sizeof(req));
    c = req.data;

    switch (req.type) {
      case TERM_GET:
        if (sh_tid == -1) {
          sh_tid = recv_tid;
          tdisp_set_active_task(&wm.td, sh_tid);
        }
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
          case ESCAPE:
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
        for (i = 0; i < req.len; ++i) {
          tdisp_write_task(&wm.td, recv_tid, req.data[i]);
        }
        Reply(recv_tid, &rep, sizeof(rep));
        break;
      case TERM_LOG_REG:
        log_tid = recv_tid;
        Reply(log_tid, &rep, sizeof(rep));
        break;
      case TERM_LOG:
        log_line++;
        if (log_line > LOG_HEIGHT - 3) {
          tdisp_write_task(&wm.td, log_tid, TERM_RETURN);
          log_line = 0;
        }
        r = buf_pack_i32(tid_buf, recv_tid);
        assert(r != 0);
        for (i = 0; i < r; ++i)
          tdisp_write_task(&wm.td, log_tid, tid_buf[i]);
        tdisp_write_task(&wm.td, log_tid, ':');
        tdisp_write_task(&wm.td, log_tid, ' ');
        for (i = 0; i < req.len; ++i)
          tdisp_write_task(&wm.td, log_tid, req.data[i]);
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
  req.type  = TERM_REG;
  config[0] = offsetx;
  config[1] = offsety;
  config[2] = width;
  config[3] = height;
  req.data  = config;
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

void TMLogRegister(tid_t tm_tid) {
  TManReq req;
  TManRep rep;
  req.type = TERM_LOG_REG;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
}

void TMLogStrf(tid_t tm_tid, char *fmt, ...) {
  int len;
  va_list va;
  char buf[100];
  TManReq req;
  TManRep rep;

  va_start(va, fmt);
  len = buf_pack_fmt(buf, fmt, va);
  va_end(va);


  req.type = TERM_LOG;
  req.data = buf;
  req.len  = len;
  Send(tm_tid, &req, sizeof(req), &rep, sizeof(rep));
}
