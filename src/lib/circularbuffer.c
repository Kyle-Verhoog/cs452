#include <circularbuffer.h>

void init_circularBuffer(CircularBuffer *buffer) {
	buffer->buffer_start = 0;
	buffer->buffer_end = 0;
}

