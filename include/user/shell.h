#ifndef SHELL_H
#define SHELL_H

#include <io.h>
#include <lib/circular_buffer.h>
#include <lib/parse.h>
#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/train/train_manager.h>
#include <user/train/switch_manager.h>
#include <user/terminal_manager.h>

#define CMD_MAX 41
#define CMD_BUF_MAX 512

#define SH_CLEAR_COUNT 3

CIRCULAR_BUFFER_DEC(cmd_cb, char, CMD_BUF_MAX);

typedef struct shell {
  char cmd[CMD_MAX];
  cmd_cb buf;
  int cur_pos;
  int len;
  int cmd_count;
  int clear_count;
  bool clear;
} shell;

void Shell();

#endif
