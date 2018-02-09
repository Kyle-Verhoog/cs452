#ifndef INTERRUPT_MATRIX_H
#define INTERRUPT_MATRIX_H
#include <defines.h>
#include <system.h>
#include <task.h>
#include <circularbuffer.h>

typedef struct interrupt_matrix{
	tid_t buffer_TC1UI[256];	// TODO: MAKE DEFINE
	CircularBuffer TC1UI;

	tid_t buffer_TC3UI[256];	// TODO: MAKE DEFINE
	CircularBuffer TC3UI;
} interrupt_matrix;

void im_init(interrupt_matrix *im);
int im_push(interrupt_matrix *im, TaskDescriptor *task, InterruptEvent ie);
TaskDescriptor *im_top(interrupt_matrix *im, InterruptEvent ie);
int im_pop(interrupt_matrix *im, InterruptEvent ie);
unsigned int im_eventsize(interrupt_matrix *im, InterruptEvent ie);

#endif /* INTERRUPT_MATRIX_H */
