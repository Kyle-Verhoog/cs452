#include <task/im_buffer.h>

CIRCULAR_BUFFER_DEF(im_cb, TaskDescriptor*, IRQ_MAX_SIZE);
