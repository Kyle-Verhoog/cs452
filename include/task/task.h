#ifndef TASK_H
#define TASK_H
#include <defines.h>
#include <types.h>
#include <system.h>
#include <asm.h>

typedef enum TaskStatus{ // a task is...
	ACTIVE  = 0,           //  active, currently running
	READY   = 1,           //  ready to be run
	BLOCKED = 2,           //  blocked, waiting for something
  UNINIT  = 3,           //  not yet initialized
  ZOMBIE  = 4,           //  dead... but still alive?
}TaskStatus;

//Kernel Handles Task Request
typedef enum TaskRequest{
	PASS = 0,
	BLOCK = 1,
	CREATE = 2,
	MY_TID = 3,
	MY_PARENT_TID = 4,
  	EXIT = 5,
}TaskRequest;

typedef struct TaskDescriptor{
	uint32_t  tid;	//Task id
	
	uint32_t  sp;		//stack pointer
	uint32_t  stack_base; //stack base

	uint32_t  psr;		//Status Register
	
	void* task;	//Function pointer
	TaskStatus status;	//Task status

	struct TaskDescriptor *parent; //Parent task

	uint32_t ret; //Return value
}TaskDescriptor;

//Active Task Global
typedef struct ActiveTask{
	int r0;
	int r1;
	int r2;
	int r3;

	//Change CPSR Location
	int psr_temp;
}ActiveTask;

ActiveTask active_task;

//Tasks
void taskOne();
void taskTwo();

//SysCalls
void Pass();
void Block();
int Create(int priority, void (*code)());
int MyTid();
int MyParentTid();
void Exit();

/**
 * Initialize a task descriptor to be uninitialized.
 */
void td_init(TaskDescriptor *td);

void td_create(TaskDescriptor *td, uint32_t tid, void *task, TaskStatus status, TaskDescriptor *parent);

// TODO: these are copies of the ones in kernel.h, we should figure out where
//       to put them centrally.

#define USER_MODE 16
#define KERNEL_MODE 19
#define SYSTEM_MODE 31
#define USER_STACK_BASE 0x02000000
	#define USER_STACK_SIZE 0x100000	//1 MB User stacks


#endif /* TASK_H */
