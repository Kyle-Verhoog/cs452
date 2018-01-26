#ifndef DEFINES_H
#define DEFINES_H

// #define DEBUG 1

#define LOG_COM 1

#ifndef NULL
#define NULL  0
#endif
#define false 0
#define true  1

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

#define MAX_TASK 16

#endif // DEFINES_H

