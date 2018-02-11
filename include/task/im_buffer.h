#ifndef IM_BUFFER_H
#define IM_BUFFER_H
#include <task/task.h>
#include <lib/circular_buffer.h>

CIRCULAR_BUFFER_DEC(im_cb, TaskDescriptor*, IRQ_MAX_SIZE);

#endif
