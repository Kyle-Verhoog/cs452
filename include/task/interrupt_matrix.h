#ifndef INTERRUPT_MATRIX_H
#define INTERRUPT_MATRIX_H
#include <defines.h>
#include <system.h>
#include <task/task.h>
#include <task/im_buffer.h>

typedef struct interrupt_matrix{
  im_cb TC1UI;
  im_cb TC3UI;
} interrupt_matrix;

void im_init(interrupt_matrix *im);
int im_push(interrupt_matrix *im, TaskDescriptor *task, InterruptEvent ie);
TaskDescriptor *im_top(interrupt_matrix *im, InterruptEvent ie);
int im_pop(interrupt_matrix *im, InterruptEvent ie, TaskDescriptor **td);
unsigned int im_eventsize(interrupt_matrix *im, InterruptEvent ie);

#endif /* INTERRUPT_MATRIX_H */
