#include <task/tid_buffer.h>

CIRCULAR_BUFFER_DEF(tid_cb, tid_t, MAX_TASK);

void Notify(tid_cb *subscribers){
	int reply = 1;
	tid_t sub;
	while(tid_cb_pop(subscribers, &sub) != CB_E_EMPTY){
		Reply(sub, &reply, sizeof(reply));
	}
}
