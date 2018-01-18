#include <kernel.h>

#define asm __asm__

//TODO: Remove Test Task
TaskDescriptor TEST_TASK;
//


/**
 * Saves the old SP to the new kernel stack and sets SP to the kernel stack.
 */
#define KERNEL_INIT() asm( \
  ".extern kernel_stack_base;" \
	"ldr r1, =kernel_stack_base;" \
	"ldr r1, [r1];" \
  "sub fp, r1, #4;" \
  "sub r1, r1, #64;" \
	"mov r2, sp;" \
	"mov sp, r1;" \
	"stmfd sp!, {r2};" \
);

/**
 * Restores the old SP in order to return to RedBoot.
 */
#define KERNEL_EXIT() asm( \
	"ldmfd sp, {sp};" \
);


unsigned int kernel_stack_base = KERNEL_STACK_BASE;
unsigned int user_stack_base = USER_STACK_BASE;

void swi_handler(){

}

void initialize() {
  bwprintf(COM2, "Initializing...\n\r");

  // Set the SWI handler to swi_handler
	//int *kep = (int *)KERNEL_ENTRY;
  //*kep = (int)(&swi_handler);
  asm(
    "ldr r3, =KERNEL_ENTRY;"
    "mov r4, #"STR(KERNEL_ENTRY)";"
    "str r3, [r4];"
  );

  //TODO: Fix Initialize task one
  TEST_TASK.tid = 0;
  TEST_TASK.sp = USER_STACK_BASE - 52;  //Initialize stack pointer down 13 registers
  TEST_TASK.stack_base = USER_STACK_BASE; 
  TEST_TASK.pc = (unsigned int)&taskOne;
  TEST_TASK.psr = 16;
  TEST_TASK.task = &taskOne;
  TEST_TASK.status = READY;


  //Put the first USER task onto the schedule as READY
}

TaskDescriptor* schedule(){
  return &TEST_TASK;
}

KernelRequest activate(TaskDescriptor* td) {
  PRINT_REG("r0");
  //Store Kernel State
  asm(
    "stmfd sp!, {r0-r12};"
  );

  // asm(
  //   "mov r5, %0;"::"r"(td)
  //   );
  // PRINT_REG("r5");
  //Set the spsr register in Kernel mode
  //bwprintf(COM2, "TD: psr %x, sp %x, pc %x \r\n", td->psr, td->sp, td->pc);

  //PRINT_PSR("spsr");

  // asm (
  //   "msr spsr, %0;"::"r"(td->psr)
  // );

  // PRINT_PSR("spsr");

  // bwprintf(COM2, "LOADED SPSR\n\r");

  // asm(
  //   "mov r5, fp;"
  // );
  // PRINT_REG("r5");

  // asm(
  //   "mov r5, %0;"::"r"(td->sp)
  // );
  // PRINT_REG("r5");

  //Change to system mode
  SET_CPSR(STR(SYSTEM_MODE));
  //Change the stack pointer to the task's stack (use arbitrary scratch register r3)
  /*asm (
    "ldr r3, [fp, #-20];"
    "ldr ip, [r3, #4];"
    "mov sp, ip;"
  );*/

  asm(
    "mov sp, %0;"::"r"(td->sp)
  );
  // PRINT_REG("r5");

  PRINT_REG("sp");

  //bwprintf(COM2, "TD: psr %x, sp %x, pc %x \r\n", td->psr, td->sp, td->pc);

  //Load the User Trap Frame
  /*
   asm(
     "ldmfd sp!, {r0-r12};"
   );*/

  PRINT_REG("sp");

  //bwprintf(COM2, "LOADED USER TRAP\n\r");

  //Switch back to kernel mode
  SET_CPSR(STR(KERNEL_MODE));

  PRINT_REG("sp");

  //Install spsr
  asm(
    "msr spsr, %0;"::"r"(td->psr)
  );

  //Move to the user task
  asm (
    "movs pc, %0;"::"r"(td->pc)
  );

  //AFTER USER TASK CALLS SWI
  asm("KERNEL_ENTRY:");

  //Save the lr_svc to TaskDescriptor's pc
  asm(
    "ldr r3, [fp, #-16];"
    "ldr r3, [r3, #12];"
    "str lr, [r3]"
  );

  //Change to System mode
  SET_CPSR(STR(SYSTEM_MODE));

  //Save the user state 
  asm(
    "stmfd sp!, {r0-r12};"
  );

  //Save the user sp to TaskDescriptor's sp
  asm(
    "ldr r3, [fp, #-16];"
    "ldr r3, [r3, #4];"
    "str sp, [r3]"
  );

  //Change back to kernel mode
  SET_CPSR(STR(KERNEL_MODE));

  //Load kernel Trap Frame
  asm(
    "ldmfd sp!, {r0-r12};"
  );

  //Return SWI Argument

  return PASS;
}

void handle(KernelRequest req){
  //Switch Statement
};

int main(void) {
  KERNEL_INIT();

  initialize();

  while(TRUE){
    //get a task from scheduler
    TaskDescriptor* td = schedule();

    //activate task
    KernelRequest req = activate(td);

    //Handle the swi
    handle(req);
  }

  KERNEL_EXIT();
	return 0;
}

