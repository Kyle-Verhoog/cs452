#ifndef KERNEL_H
#define KERNEL_H

#include <bwio.h>
#include <ts7200.h>
#include <task.h>

//Data Structures
#include <circularbuffer.h>

//Debug - set DEBUG_ON through the gcc option (-D DEBUG_ON)
#ifdef DEBUG_ON
	#include <debug.h>
#endif /* DEBUG_ON */

#define FALSE 0
#define TRUE 1

//Kernel Defines
#define KERNEL_ENTRY 0x28

#define KERNEL_STACK_BASE 0x01000000 //0x01000000
//#define KERNEL_STACK_BASE 0x01000000 //0x01000000

#define USER_STACK_BASE 0x02000000
	#define USER_STACK_SIZE_OFFSET 0x100000	//1 MB User stacks

//Kernel Modes
#define USER_MODE 16
#define KERNEL_MODE 19
#define SYSTEM_MODE 31

//Kernel Handler
typedef enum KernelRequest{
	PASS,
	BLOCK
}KernelRequest;

//Bridges from C to ASM
extern unsigned int kernel_stack_base;
extern unsigned int user_stack_base;

//Macros
#define PRINT_REG(reg)   asm(				\
						   "mov  r0, #1;" 	\
						   "mov  r1, "reg";"\
						   "bl   bwputr;"	\
						  );

#define PRINT_PSR(psr)   asm(				\
						   "mov  r0, #1;" 	\
						   "mrs  r1, "psr";"\
						   "bl   bwputr;"	\
						  );

#define SET_CPSR(mode) asm( \
		"mrs r0, cpsr;" \
		"bic r0, r0, #31;" \
		"orr r0, r0, #"mode";" \
		"msr cpsr, r0;" \
	); \

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#endif /* KERNEL_H */
