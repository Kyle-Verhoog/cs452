#ifndef KERNEL_H
#define KERNEL_H

#include <defines.h>
#include <bwio.h>
#include <system.h>
#include <ts7200.h>
#include <asm.h>
#include <kernel_task.h>
#include <user/test/k1_task.h>
#include <user/test/k2_task.h>
#include <user/test/k2_metric.h>
#include <user/test/k3_task.h>

#ifdef KTEST
#include <user/test/test_task.h>
#endif

//Data Structures
#include <circularbuffer.h>
#include <priority_queue.h>

//Debug - set DEBUG through the gcc option (-D DEBUG)
#include <debug.h>

#include <kernel/handlers/msg.h>
#include <kernel/handlers/nameserver.h>


//Bridges from C to ASM
extern unsigned int kernel_stack_base;
extern unsigned int user_stack_base;

void send_handler(TaskDescriptor *std);
void receive_handler(TaskDescriptor *rtd);

TaskDescriptor tasks[MAX_TASK];
TidTracker tid_tracker;
priority_queue pq_tasks;


#endif /* KERNEL_H */
