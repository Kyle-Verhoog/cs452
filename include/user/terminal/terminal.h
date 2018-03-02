#ifndef TERMINAL_H
#define TERMINAL_H

#include <lib/circular_buffer.h>

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
} TWindow;

typedef struct TerminalDisplay {
  TCursor tdcur;                    // cursor used for tracking cursor
  tdisp_cb buffer;                  // buffer of commands to PutC/PutStr
  wid_cb avail_wids;                // available window ids
  TWindow windows[MAX_WINDOWS];
  bool active_windows[MAX_WINDOWS]; // active windows
  int num_active_windows;           // number of active windows
  TWindow *focused_window;          // the current window
} TDisplay;

void tdisp_init(TDisplay *td);

void tdisp_add_window(TDisplay *td, int x, int y, int w, int h);

void tdisp_focus_window(TDisplay *td, int wid);

void tdisp_delete_window(TDisplay *td);

void tdisp_writec(TDisplay *td, char c);
#endif
