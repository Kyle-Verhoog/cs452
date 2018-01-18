#ifndef TASK_H
#define TASK_H

typedef enum TaskStatus{
	BLCOKED = 0,
	READY = 1,
	ACTIVE = 2
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

#endif /* TASK_H */
