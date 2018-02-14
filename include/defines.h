#ifndef DEFINES_H
#define DEFINES_H

#define LOG_COM 0

#ifndef NULL
#define NULL  0
#endif
#define false 0
#define true  1

//Kernel Defines
#define SWI_ENTRY 0x28
#define IRQ_ENTRY 0x38

#define IRQ_STACK_BASE 0x100000
#define KERNEL_STACK_BASE 0x01000000 //0x01000000

#define USER_STACK_BASE 0x02000000
#define USER_STACK_SIZE 0x100000  //1 MB User stacks

//Kernel Modes
#define USER_MODE 16
#define IRQ_MODE 18
#define KERNEL_MODE 19 
#define SYSTEM_MODE 31 

// The number of tasks in the system
#define MAX_TASK  16
#define MAX_PRIORITY 32
#define NS_MAX_ENTRY 64
#define CSQ_MAX_SIZE MAX_TASK
#define IRQ_MAX_SIZE MAX_TASK


// User Task ID defines
#define CLOCKSERVER_ID 2
#define IOSERVER_ID    3

#endif // DEFINES_H

