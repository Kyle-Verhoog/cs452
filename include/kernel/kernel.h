#ifndef KERNEL_H
#define KERNEL_H

#include <bwio.h>
#include <defines.h>
#include <system.h>
#include <ts7200.h>

//Data Structures
#include <circularbuffer.h>
#include <task_queue.h>

//Debug - set DEBUG through the gcc option (-D DEBUG)
#ifdef DEBUG
	#include <debug.h>
#endif /* DEBUG_ON */


//Kernel Defines
#define KERNEL_ENTRY 0x28

#define KERNEL_STACK_BASE 0x01000000 //0x01000000
//#define KERNEL_STACK_BASE 0x01000000 //0x01000000

#define USER_STACK_BASE 0x02000000
	#define USER_STACK_SIZE 0x100000	//1 MB User stacks

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
		"orr r0, r0, #"STR(mode)";" \
		"msr cpsr, r0;" \
	);

#define PUSH_STACK(param)	asm( \
					    		"stmfd sp!, {"param"};" \
					  		);

#define POP_STACK(param)	asm ( \
								"ldmfd sp!, {"param"};" \
							);	

#define WRITE_SP(val)	asm( \
					    	"mov sp, %0;"::"r"(val) \
						);

#define WRITE_SPSR(val) asm( \
							"msr spsr, %0;"::"r"(val) \
						);

#define READ_SP(var) 	asm ( \
							"mov %0, sp;":"=r"(var): \
						);

#define READ_SPSR(var)	asm ( \
							"mrs %0, spsr;":"=r"(var): \
						);

#define REVERSE_SWI() 	asm( \
							"movs pc, lr" \
						)

#endif /* KERNEL_H */
