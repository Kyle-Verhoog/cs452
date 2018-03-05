#ifndef INTERRUPT_MATRIX_H
#define INTERRUPT_MATRIX_H
#include <defines.h>
#include <system.h>
#include <task/task.h>
#include <task/im_buffer.h>

typedef struct interrupt_matrix{
  im_cb TC1UI;
  im_cb TC3UI;

  im_cb UART2_RX;
  im_cb UART2_TX;
  im_cb UART2_RT;
  im_cb UART2_MI;

  im_cb UART1_RX;
  im_cb UART1_TX;
  im_cb UART1_RT;
  im_cb UART1_MI;

  bool stored_flag[1024];
} interrupt_matrix;

void im_init(interrupt_matrix *im);
int im_push(interrupt_matrix *im, TaskDescriptor *task, InterruptEvent ie);
int im_pop(interrupt_matrix *im, InterruptEvent ie, TaskDescriptor **td);
int im_eventsize(interrupt_matrix *im, InterruptEvent ie);

#endif /* INTERRUPT_MATRIX_H */
