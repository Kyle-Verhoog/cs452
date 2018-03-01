#ifndef KERNEL_H
#define KERNEL_H

#include <defines.h>
#include <system.h>
#include <ts7200.h>
#include <asm/asm.h>
#include <kernel_task.h>

#include <user/test/k2_task.h>
#include <user/test/k3_task.h>

#ifdef KTEST
#include <user/test/test_task.h>
#endif
#include <user/bootstrap.h>
#include <user/test/ioserver_test.h>

//Data Structures
#include <circularbuffer.h>
#include <priority_queue.h>
#include <interrupt_matrix.h>

//Debug - set DEBUG through the gcc option (-D DEBUG)
#include <debug.h>

#include <kernel/handlers/create.h>
#include <kernel/handlers/msg.h>
#include <kernel/handlers/nameserver.h>
#include <kernel/handlers/interrupt.h>


//Bridges from C to ASM
extern unsigned int kernel_stack_base;
extern unsigned int user_stack_base;

void send_handler(TaskDescriptor *std);
void receive_handler(TaskDescriptor *rtd);

TaskDescriptor tasks[MAX_TASK];
TidTracker tid_tracker;
priority_queue pq_tasks;
interrupt_matrix im_tasks;


#endif /* KERNEL_H */
