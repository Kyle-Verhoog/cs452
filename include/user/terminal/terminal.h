#ifndef TERMINAL_H
#define TERMINAL_H

#include <lib/circular_buffer.h>
#include <ascii.h>

#ifndef X86
#include <user/syscalls.h>
#endif

#define DISPLAY_WIDTH  300
#define DISPLAY_HEIGHT 300
#define MAX_WINDOWS 4
#define TERMINAL_BUFFER_SIZE 1024

CIRCULAR_BUFFER_DEC(tdisp_cb, char, TERMINAL_BUFFER_SIZE);
CIRCULAR_BUFFER_DEC(wid_cb, int, MAX_WINDOWS);

typedef struct TerminalCursor {
  int x;
  int y;
} TCursor;

typedef struct TerminalWindow {
  int wid;
  TCursor cur;
  int offsetx;
  int offsety;
  int w;
  int h;
  tid_t tid;
} TWindow;

typedef struct TerminalDisplay {
  TCursor tdcur;                    // cursor used for tracking cursor
  tdisp_cb buffer;                  // buffer of commands to PutC/PutStr
  wid_cb avail_wids;                // available window ids
  TWindow windows[MAX_WINDOWS];
  int task_window[64];         // mapping from tid_id to window
  bool active_windows[MAX_WINDOWS]; // active windows
  int num_active_windows;           // number of active windows
  TWindow *focused_window;          // the current window
} TDisplay;

void tdisp_init(TDisplay *td);

int tdisp_add_window(TDisplay *td, int x, int y, int w, int h, tid_t tid);

void tdisp_focus_window(TDisplay *td, int wid);

void tdisp_delete_window(TDisplay *td);

void tdisp_writec(TDisplay *td, char c);

void tdisp_write_task(TDisplay *td, tid_t tid, char c);
#endif
