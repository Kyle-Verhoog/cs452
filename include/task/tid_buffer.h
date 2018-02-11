#ifndef TID_BUFFER_H
#define TID_BUFFER_H
#include <defines.h>
#include <lib/circular_buffer.h>

CIRCULAR_BUFFER_DEC(tid_cb, tid_t, MAX_TASK);

#endif
