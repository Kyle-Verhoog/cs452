#ifndef TASK_H
#define TASK_H
#include <defines.h>
#include <types.h>
#include <system.h>

typedef enum TaskStatus{ // a task is...
	ACTIVE  = 0,           //  active, currently running
	READY   = 1,           //  ready to be run
	BLOCKED = 2,           //  blocked, waiting for something
  UNINIT  = 3,           //  not yet initialized
  ZOMBIE  = 4,           //  dead... but still alive?
}TaskStatus;

typedef struct TaskDescriptor{
	unsigned int tid;	//Task id
	
	unsigned int sp;		//stack pointer
	unsigned int stack_base; //stack base

	unsigned int psr;		//Status Register
	
	void* task;	//Function pointer
	TaskStatus status;	//Task status

}TaskDescriptor;

//Tasks
void taskOne();
void taskTwo();

/**
 * Initialize a task descriptor to be uninitialized.
 */
void td_init(TaskDescriptor *td);

void td_create(TaskDescriptor *td, uint32_t tid, void *task, TaskStatus status);

// TODO: these are copies of the ones in kernel.h, we should figure out where
//       to put them centrally.

#define USER_MODE 16
#define KERNEL_MODE 19
#define SYSTEM_MODE 31
#define USER_STACK_BASE 0x02000000
	#define USER_STACK_SIZE 0x100000	//1 MB User stacks
#define SET_CPSR(mode) asm( \
  "mrs r0, cpsr;" \
  "bic r0, r0, #31;" \
  "orr r0, r0, #"STR(mode)";" \
  "msr cpsr, r0;" \
);

#define PUSH_STACK(param)	asm( \
  "stmfd sp!, {"param"};" \
);

#define WRITE_SP(val)	asm( \
  "mov sp, %0;"::"r"(val) \
);


#endif /* TASK_H */
