#include <user/shell.h>
#include <stoppingcalibration_test.h> //TODO:Remove this
#include <stop_at_test.h> //TODO:Remove this

CIRCULAR_BUFFER_DEF(cmd_cb, char, CMD_BUF_MAX);

int sh_args[4]; // kinda a bad hack

void ShellDummyTask() {
  tid_t tm_tid;
  int i, j, k, x, y, w, h;

  x = sh_args[0];
  y = sh_args[1];
  w = sh_args[2];
  h = sh_args[3];

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);
  TMRegister(tm_tid, x, y, w, h);

  i = j = k = 0;
  while (true) {
    i++;
    if (i > 2000000) {
      if (j == w-2) {
        TMPutC(tm_tid, '\n');
        if (k == h-2) {
          TMPutC(tm_tid, '\r');
          k = 0;
        } else {
          k++;
        }
        j = 0;
      } else {
        TMPutC(tm_tid, 'a'+j);
        j++;
      }
      i = 0;
    }
  }
}

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
  cmd_cb_push_str(&sh->buf, "\nsh: '");
  cmd_cb_push_str(&sh->buf, msg);
  cmd_cb_push_str(&sh->buf, "'\n");
}

void shell_error(shell *sh) {
  cmd_cb_push_str(&sh->buf, "\nsh: unknown cmd `");
  cmd_cb_push_str(&sh->buf, sh->cmd);
  cmd_cb_push_str(&sh->buf, "`\n");
}

void shell_errorf(shell *sh, char *msg) {
  cmd_cb_push_str(&sh->buf, "\ncmd err: '");
  cmd_cb_push_str(&sh->buf, msg);
  cmd_cb_push_str(&sh->buf, "'\n");
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
  int arg1, arg2, arg3, arg4;

  tid_t tr_tid, sw_tid, sc_tid, sa_tid, pm_tid;

  tr_tid = WhoIs(TRAIN_MANAGER_ID);
  sw_tid = WhoIs(SWITCH_MANAGER_ID);
  sc_tid = WhoIs(STOPPING_CALIBRATION_ID);
  sa_tid = WhoIs(STOP_AT_SERVER_ID);
  pm_tid = WhoIs(PREDICTION_MANAGER_ID);
  assert(tr_tid > 0);
  assert(sw_tid > 0);
  assert(sc_tid > 0);
  assert(sa_tid > 0);
  assert(pm_tid > 0);

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
      tm.arg1 = arg1; //train
      tm.arg2 = arg2; //train speed
      Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
      shell_info(sh);
    }
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
  else if (cmd[0] == 'm' && cmd[1] == 's') {
    if ((r = parse_i32(cmd+2, &arg1)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_errorf(sh, "train number");
    }
    else {
      TMProtocol tm;
      tm.tmc = TM_MEASURE;
      tm.arg1 = arg1; //train
      Send(tr_tid, &tm, sizeof(tm), &reply, sizeof(reply));
      shell_info(sh);
    }
  }
  else if(cmd[0] == 's' && cmd[1] == 'c'){
    if ((r = parse_i32(cmd+2, &arg1)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_errorf(sh, "train number");
    }
    else if ((r = parse_i32(r, &arg2)) == 0) {
      shell_errorf(sh, "train cmd");
    }
    else if ((r = parse_i32(r, &arg3)) == 0) {
      shell_errorf(sh, "track stop from");
    }
    else {
      StoppingCalibrationArgs scargs;
      scargs.train = arg1;
      scargs.speed = arg2;
      scargs.before_sensor = arg3;

      Send(sc_tid, &scargs, sizeof(scargs), &reply, sizeof(reply));
      shell_info(sh);
    }
  }
  else if(cmd[0] == 's' && cmd[1] == 'a'){
    if ((r = parse_i32(cmd+2, &arg1)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_errorf(sh, "train number");
    }
    else if ((r = parse_i32(r, &arg2)) == 0) {
      shell_errorf(sh, "train cmd");
    }
    else if ((r = parse_i32(r, &arg3)) == 0) {
      shell_errorf(sh, "track stop at");
    }
    else {
      StopAtProtocol sap;
      sap.sar = SAR_STOP;
      sap.train = arg1;
      sap.gear = arg2;
      sap.stop_at = arg3;
      sap.dist = -1;

      Send(sa_tid, &sap, sizeof(sap), &reply, sizeof(reply));
      shell_info(sh);
    }
  }
  else if(cmd[0] == '2' && cmd[1] == 's' && cmd[2] == 'a'){
    if ((r = parse_i32(cmd+3, &arg1)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_errorf(sh, "train number");
    }
    else if ((r = parse_i32(r, &arg2)) == 0) {
      shell_errorf(sh, "train cmd");
    }
    else if ((r = parse_i32(r, &arg3)) == 0) {
      shell_errorf(sh, "track stop at");
    }
    else if ((r = parse_i32(r, &arg4)) == 0) {
      shell_errorf(sh, "specified distance");
    }
    else {
      StopAtProtocol sap;
      sap.sar = SAR_STOP;
      sap.train = arg1;
      sap.gear = arg2;
      sap.stop_at = arg3;
      sap.dist = arg4;

      Send(sa_tid, &sap, sizeof(sap), &reply, sizeof(reply));
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
      int sh_args[4];
      sh_args[0] = x;
      sh_args[1] = y;
      sh_args[2] = w;
      sh_args[3] = h;
      Create(29, &Shell);
    }
  }
  else if (cmd[0] == 'c' && cmd[1] == 'r') {
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
      shell_info_msg(sh, "spawning test window");
      sh_args[0] = x;
      sh_args[1] = y;
      sh_args[2] = w;
      sh_args[3] = h;
      Create(1, &ShellDummyTask);
    }
  }
  else if (cmd[0] == 'c' && cmd[1] == 'l' && cmd[2] == 's') {
    shell_clear(sh);
    return;
  }
  else if (cmd[0] == 'r' && cmd[1] == 't') {
    if ((r = parse_i32(cmd+2, &arg1)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_errorf(sh, "rt <tr> <spd>");
    }
    else if ((r = parse_i32(r, &arg2)) == 0 || arg1 > 81 || arg1 < 0) {
      shell_errorf(sh, "rt <tr> <spd>");
    }
    else {
      shell_info_msg(sh, "spawning m1 reset");
      sh_args[0] = arg1;
      sh_args[1] = arg2;
      CreateArgs(30, &M1Reset, (void *)sh_args);
    }
  }
  else if (cmd[0] == 's' && cmd[1] == 't' && cmd[2] == 'p') {
    int x, y;
    if ((r = parse_i32(cmd+3, &x)) == 0) {
      shell_error(sh);
    }
    else if ((r = parse_i32(r, &y)) == 0) {
      shell_error(sh);
    }
    else {
      shell_info_msg(sh, "spawning milestone 1");
      sh_args[0] = x;
      sh_args[1] = y;
      // Create(28, &Pathing);
    }
  }
  else if (cmd[0] == 'g' && cmd[1] == 'o') {
    PMProtocol pmp;
    int s, e;
    if ((r = parse_i32(cmd+2, &s)) == 0 && s >= 0 && s <= TRACK_MAX) {
      shell_error(sh);
    }
    else if ((r = parse_i32(r, &e)) == 0 && e >= 0 && e <= TRACK_MAX) {
      shell_error(sh);
    }
    else {
      shell_info_msg(sh, "routing train");
      sh_args[0] = s;
      sh_args[1] = e;
      pmp.pmc = PM_ROUTE;
      pmp.args = (void *)sh_args;
      Send(pm_tid, &pmp, sizeof(pmp), &reply, sizeof(reply));
    }
  }
  else {
    shell_error(sh);
  }

  shell_reset(sh);
}


void Shell(void *args) {
  tid_t tm_tid;
  char c;
  shell sh;
  int *vars;
  int x, y, w, h;

  vars = (int *)args;
  x = vars[0];
  y = vars[1];
  w = vars[2];
  h = vars[3];

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  //TMRegister(tm_tid, SH_OFFSET_X, SH_OFFSET_Y, SH_WIDTH, SH_HEIGHT);
  TMRegister(tm_tid, x, y, w, h);

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
