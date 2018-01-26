#ifndef KERNEL_H
#define KERNEL_H

#include <bwio.h>
#include <defines.h>
#include <system.h>
#include <ts7200.h>
#include <asm.h>
#include <kernel_task.h>
#include <user/test/k1_task.h>
#include <user/test/k2_task.h>

#ifdef KTEST
#include <user/test/test_task.h>
#endif

//Data Structures
#include <circularbuffer.h>
#include <priority_queue.h>

//Debug - set DEBUG through the gcc option (-D DEBUG)
#include <debug.h>


//Kernel Defines
#define KERNEL_ENTRY 0x28

#define KERNEL_STACK_BASE 0x01000000 //0x01000000
//#define KERNEL_STACK_BASE 0x01000000 //0x01000000

#define USER_STACK_BASE 0x02000000
#define USER_STACK_SIZE 0x100000  //1 MB User stacks

//Kernel Modes
#define USER_MODE 16
#define KERNEL_MODE 19
#define SYSTEM_MODE 31

//Bridges from C to ASM
extern unsigned int kernel_stack_base;
extern unsigned int user_stack_base;

#endif /* KERNEL_H */
