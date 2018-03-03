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
  sh->count = 0;
  cmd_cb_init(&sh->buf);

  shell_prompt(sh);
}

void shell_print(shell *sh, tid_t tm_tid) {
  char c;
  while (sh->buf.size > 0) {
    cmd_cb_pop(&sh->buf, &c);
    TMPutC(tm_tid, c);
  }
}

void shell_add_c(shell *sh, char c) {
  int i, pos;

  if (sh->len + 1 > CMD_MAX)
    return;

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

void shell_error(shell *sh) {
  cmd_cb_push_str(&sh->buf, "\nsh: unknown cmd `");
  cmd_cb_push_str(&sh->buf, sh->cmd);
  cmd_cb_push_str(&sh->buf, "`\n");
}

void shell_reset(shell *sh) {
  shell_clear_cmd(sh);
  if (sh->count == 3) {
    cmd_cb_push(&sh->buf, '\r');
    sh->count = 0;
  }
  shell_prompt(sh);
}

void shell_exec(shell *sh) {
  char *cmd;
  char *r;
  int arg1, arg2;

  cmd = sh->cmd;

  if (cmd[0] == 'q') {
    Halt();
  }
  else if (cmd[0] == 't' && cmd[1] == 'r') {
    if ((r = parse_i32(cmd+2, &arg1)) == 0) {
      shell_error(sh);
    }
    else if ((r = parse_i32(r, &arg2)) == 0 || arg2 > 81 || arg2 < 0) {
      shell_error(sh);
    }
    else {
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
      shell_info(sh);
    }
  }
  else {
    shell_error(sh);
  }

  sh->count++;
  shell_reset(sh);
}


void Shell() {
  tid_t tm_tid;
  char c;
  shell sh;

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  TMRegister(tm_tid, 2, 1, 47, 9);

  shell_init(&sh);
  shell_print(&sh, tm_tid);

  while (true) {
    c = TMGetC(tm_tid);
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
