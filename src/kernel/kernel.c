#include <system.h>
#include <kernel.h>

#define asm __asm__

//TODO: Remove Test Task
TaskDescriptor TEST_TASK;
//


/**
 * Saves the old SP to the new kernel stack and sets SP to the kernel stack.
 */
#define KERNEL_INIT() asm(      \
  ".extern kernel_stack_base;"  \
	"ldr r1, =kernel_stack_base;" \
	"ldr r1, [r1];"               \
  "sub fp, r1, #4;"             \
  "sub r1, r1, #64;"            \
	"mov r2, sp;"                 \
	"mov sp, r1;"                 \
	"stmfd sp!, {r2};"            \
);

/**
 * Restores the old SP in order to return to RedBoot.
 */
#define KERNEL_EXIT() asm( \
  "KERNEL_EXIT:"           \
	"ldmfd sp, {sp};"        \
);


unsigned int kernel_stack_base = KERNEL_STACK_BASE;
unsigned int user_stack_base = USER_STACK_BASE;

void initialize() {
  bwprintf(COM2, "Initializing...\n\r");

  asm(
    "ldr r3, =KERNEL_ENTRY;"
    "mov r4, #"STR(KERNEL_ENTRY)";"
    "str r3, [r4];"
  );

  //TODO: Fix Initialize task one
  TEST_TASK.tid = 0;
  TEST_TASK.sp = USER_STACK_BASE - 56;  //Save r0-12, lr
  TEST_TASK.stack_base = USER_STACK_BASE; 
  TEST_TASK.psr = 16;
  TEST_TASK.task = &taskOne;
  TEST_TASK.status = READY;

  //Initialize the Test task pc
  SET_CPSR(SYSTEM_MODE);
  WRITE_SP(TEST_TASK.sp);
  asm(
    "mov r3, %0;"::"r"(TEST_TASK.task)
  );
  PUSH_STACK("r3");
  SET_CPSR(KERNEL_MODE);
  TEST_TASK.sp = TEST_TASK.sp - 4; //saved lr_svc

  //Put the first USER task onto the schedule as READY
}

TaskDescriptor* schedule(){
  return &TEST_TASK;
}

KernelRequest activate(TaskDescriptor* td) {
  //Store Kernel State
  PUSH_STACK("r0-r12");
  //Install SPSR
  WRITE_SPSR(td->psr);
  //Change to system mode
  SET_CPSR(SYSTEM_MODE);
  //Change the stack pointer to the task's stack (uses fp so no worries)
  WRITE_SP(td->sp);
  //Load instruction after swi (r3) from user stack
  POP_STACK("r3");
  //Change to kernel mode
  SET_CPSR(KERNEL_MODE);
  //Save into kernel lr for loading
  asm("mov lr, r3;");
  //Change to system mode
  SET_CPSR(SYSTEM_MODE);
  //Load the User Trap Frame
  POP_STACK("r0-r12, lr");
  //Switch back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //Move to the user task
  REVERSE_SWI();

  //AFTER USER TASK CALLS SWI
  asm("KERNEL_ENTRY:");

  //Change to System mode
  SET_CPSR(SYSTEM_MODE);
  //Save the user state
  PUSH_STACK("r0-r12, lr");
  //Change to Kernel mode
  SET_CPSR(KERNEL_MODE);
  //Save lr to stratch r3
  asm("mov r3, lr");
  //Change to System mode
  SET_CPSR(SYSTEM_MODE);
  //Save the lr(r3)
  PUSH_STACK("r3")
  //Change back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //load the kernel stack (fp is now resuable again!)
  POP_STACK("r0-r12");
  //Change back to system mode
  SET_CPSR(SYSTEM_MODE); //Note we can still use fp!
  //Save the user sp to TaskDescriptor's sp
  READ_SP(td->sp);
  //Change back to kernel mode
  SET_CPSR(KERNEL_MODE);
  //Save the spsr to the TaskDescriptor's psr
  READ_SPSR(td->psr);

  //Return SWI Argument

  return PASS;
}

void handle(KernelRequest req) {
  //Switch Statement
};

int main(void) {
  KERNEL_INIT();

  initialize();

  while(true) {
    //get a task from scheduler
    TaskDescriptor* td = schedule();

    KASSERT(td != NULL);

    //activate task
    KernelRequest req = activate(td);

    //Handle the swi
    handle(req);
  }

  KERNEL_EXIT();
	return 0;
}

