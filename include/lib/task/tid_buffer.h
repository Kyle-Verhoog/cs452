#ifndef TID_BUFFER_H
#define TID_BUFFER_H
#include <defines.h>
#include <lib/circular_buffer.h>
#include <user/syscalls.h>

CIRCULAR_BUFFER_DEC(tid_cb, tid_t, MAX_TASK);
void Notify(tid_cb *subscribers);

#endif
