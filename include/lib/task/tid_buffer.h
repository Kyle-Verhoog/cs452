#ifndef TID_BUFFER_H
#define TID_BUFFER_H
#include <defines.h>
#include <lib/circular_buffer.h>
#include <user/syscalls.h>

CIRCULAR_BUFFER_DEC(tid_cb, tid_t, MAX_TASK);

#define NOTIFY(SUBS, SUB, DATA, SIZE) \
	while(tid_cb_pop(SUBS, SUB) != CB_E_EMPTY){ \
		Reply(notif_temp__, &DATA, SIZE)	\
	}	\

#endif
