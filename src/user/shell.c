#include <user/shell.h>

CIRCULAR_BUFFER_DEF(cmd_cb, char, CMD_BUF_MAX);

void cmd_cb_push_str(cmd_cb *buf, char *str) {
  while (*str != '\0') {
    cmd_cb_push(buf, *str++);
  }
}

void shell_clear_cmd(shell *sh) {
  int i;

  for (i = 0; i < CMD_MAX; ++i)
    sh->cmd[i] = '\0';

  sh->cmd[CMD_MAX-1] = '\0';
  sh->cur_pos = 0;
  sh->len = 0;
}

void shell_prompt(shell *sh) {
  cmd_cb_push_str(&sh->buf, " $ ");
}

void shell_init(shell *sh) {
  shell_clear_cmd(sh);

  sh->cur_pos = 0;
  sh->len = 0;
  sh->cmd_count = 0;
  sh->clear_count = SH_CLEAR_COUNT;
  sh->clear = false;
  cmd_cb_init(&sh->buf);

  shell_prompt(sh);
}

void shell_clear(shell *sh) {
  shell_clear_cmd(sh);
  cmd_cb_push(&sh->buf, TERM_RETURN);
  sh->cmd_count = 0;
  shell_prompt(sh);
}

void shell_pre(shell *sh) {
  if (sh->cmd_count == sh->clear_count) {
    shell_clear(sh);
  }
}

void shell_print(shell *sh, tid_t tm_tid) {
  char c;
  char buf[10];
  int i;

  i = 0;
  while (sh->buf.size > 0) {
    cmd_cb_pop(&sh->buf, &c);
    buf[i++] = c;
    if (i == 9) {
      TMPutStr(tm_tid, buf, i);
      i = 0;
    }
  }

  if (i > 0)
    TMPutStr(tm_tid, buf, i);
}

void shell_add_c(shell *sh, char c) {
  int i, pos;

  if (sh->len > CMD_MAX - 2)
    return;

  if (sh->cmd_count > sh->clear_count) {
    shell_clear(sh);
  }

  pos = sh->cur_pos;

  for (i = sh->len; i > pos; --i)
    sh->cmd[i] = sh->cmd[i-1];

  sh->cmd[pos] = c;

  sh->len++;
  for (i = pos; i < sh->len; ++i) {
    cmd_cb_push(&sh->buf, sh->cmd[i]);
  }
  sh->cur_pos++;
}

void shell_rm_c(shell *sh) {
  int i, pos;
  pos = sh->cur_pos;
  for (i = pos; i < sh->len; ++i)
    cmd_cb_push(&sh->buf, ' ');
  for (i = pos; i < sh->len; ++i)
    cmd_cb_push(&sh->buf, BACKSPACE);
  for (i = pos; i < sh->len-1; ++i) {
    sh->cmd[i] = sh->cmd[i+1];
    cmd_cb_push(&sh->buf, sh->cmd[i-1]);
  }
  sh->cmd[sh->len-1] = '\0';

  sh->len--;
}

void shell_backspace(shell *sh) {
  if (sh->cur_pos < 1)
    return;
  sh->cur_pos--;
  cmd_cb_push(&sh->buf, BACKSPACE);
  shell_rm_c(sh);
}

void shell_info(shell *sh) {
  cmd_cb_push_str(&sh->buf, "\nsh: exec `");
  cmd_cb_push_str(&sh->buf, sh->cmd);
  cmd_cb_push_str(&sh->buf, "`\n");
}

void shell_skip(shell *sh) {
  cmd_cb_push_str(&sh->buf, "\n\n");
}

void shell_info_msg(shell *sh, char *msg) {
  cmd_cb_push_str(&sh->buf, "\nsh: `");
  cmd_cb_push_str(&sh->buf, msg);
  cmd_cb_push_str(&sh->buf, "`\n");
}

void shell_error(shell *sh) {
  cmd_cb_push_str(&sh->buf, "\nsh: unknown cmd `");
  cmd_cb_push_str(&sh->buf, sh->cmd);
  cmd_cb_push_str(&sh->buf, "`\n");
}

void shell_errorf(shell *sh, char *msg) {
  cmd_cb_push_str(&sh->buf, "\nerror: `");
  cmd_cb_push_str(&sh->buf, sh->cmd);
  cmd_cb_push_str(&sh->buf, msg);
  cmd_cb_push_str(&sh->buf, "`\n");
}


void shell_reset(shell *sh) {
  sh->cmd_count++;
  shell_clear_cmd(sh);

  if (sh->cmd_count < sh->clear_count) {
    shell_prompt(sh);
  }
}

void shell_exec(shell *sh) {
  int reply;
  char *cmd;
  char *r;
  int arg1, arg2;

  tid_t tr_tid, sw_tid;

  tr_tid = WhoIs(TRAIN_MANAGER_ID);
  sw_tid = WhoIs(SWITCH_MANAGER_ID);
  assert(tr_tid > 0);
  assert(sw_tid > 0);

  cmd = sh->cmd;

  if (sh->len == 0) {
    shell_skip(sh);
  }
  else if (cmd[0] == 'q' && sh->len == 1) {
    Halt();
  }
  else if (cmd[0] == 't' && cmd[1] == 'r') {
    if ((r = parse_i32(cmd+2, &arg1)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_errorf(sh, "train number");
    }
    else if ((r = parse_i32(r, &arg2)) == 0) {
      shell_errorf(sh, "train cmd");
    }
    else {
      TMProtocol tm;
      tm.tmc = TM_MOVE;
      tm.arg1 = arg1;	//train
      tm.arg2 = arg2;	//train speed
      Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
      shell_info(sh);
    }
  }
  else if (cmd[0] == 'g' && cmd[1] == 'o') {
    shell_info(sh);
  }
  else if (cmd[0] == 'r' && cmd[1] == 'v') {
    if ((r = parse_i32(cmd+2, &arg1)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_error(sh);
    }
    else {
      TMProtocol tm;
      tm.tmc = TM_REVERSE;
      tm.arg1 = arg1; //train
      Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));

      shell_info(sh);
    }
  }
  else if (cmd[0] == 's' && cmd[1] == 'w') {
    char swd;
    if ((r = parse_i32(cmd+2, &arg1)) == 0 || arg1 < 0) {
      shell_error(sh);
    }
    else if ((r = parse_c(r, &swd)) == 0 || (swd != 'S' && swd != 'C')) {
      shell_error(sh);
    }
    else {
      SWProtocol sw;
      sw.swr = SW_FLIP;
      sw.sw = arg1;
      sw.dir = swd == 'C' ? SW_CURVE : SW_STRAIGHT;
      Send(sw_tid, &sw, sizeof(sw), &reply, sizeof(reply));
      shell_info(sh);
    }
  }
  else if (cmd[0] == 't' && cmd[1] == 'k') {
    if ((r = parse_i32(cmd+2, &arg1)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_errorf(sh, "train number");
    }
    else if ((r = parse_i32(r, &arg2)) == 0) {
      shell_errorf(sh, "train cmd");
    }
    else {
      TMProtocol tm;
      tm.tmc = TM_TRACK;
      tm.arg1 = arg1; //train
      tm.arg2 = arg2; //train track
      Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
      shell_info(sh);
    }
  }
  else if (cmd[0] == 's' && cmd[1] == 'h') {
    int x, y, w, h;
    if ((r = parse_i32(cmd+2, &x)) == 0) {
      shell_error(sh);
    }
    else if ((r = parse_i32(r, &y)) == 0) {
      shell_error(sh);
    }
    else if ((r = parse_i32(r, &w)) == 0) {
      shell_error(sh);
    }
    else if ((r = parse_i32(r, &h)) == 0) {
      shell_error(sh);
    }
    else {
      shell_info_msg(sh, "spawning new shell");
      // Create(29, &Shell);
    }
  }
  else if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 's') {
    shell_clear(sh);
    return;
  }
  else {
    shell_error(sh);
  }

  shell_reset(sh);
}



void Shell() {
  tid_t tm_tid;
  char c;
  shell sh;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  TMRegister(tm_tid, SH_OFFSET_X, SH_OFFSET_Y, SH_WIDTH, SH_HEIGHT);

  shell_init(&sh);
  shell_print(&sh, tm_tid);

  while (true) {
    c = TMGetC(tm_tid);
    shell_pre(&sh);
    switch (c) {
      case CARRIAGE_RETURN:
        shell_exec(&sh);
        break;
      case BACKSPACE:
        shell_backspace(&sh);
        break;
      default:
        shell_add_c(&sh, c);
        break;
    }
    shell_print(&sh, tm_tid);
  }
}
