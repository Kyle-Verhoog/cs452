#ifndef HANDLER_INTERRUPT_H
#define HANDLER_INTERRUPT_H

#include <kernel.h>
#include <interrupt_matrix.h>


void event_register( interrupt_matrix *im, TaskDescriptor *td);
void event_wake(interrupt_matrix *im);


#endif /*HANDLER_INTERRUPT_H*/
