#include <kernel.h>

#define asm __asm__

/**
 * Saves the old SP to the new kernel stack and sets SP to the kernel stack.
 */
#define KERNEL_INIT() asm( \
  ".extern kernel_stack_base;" \
	"ldr r1, =kernel_stack_base;" \
	"ldr r1, [r1];" \
	"mov r2, sp;" \
	"mov sp, r1;" \
	"stmed sp!, {r2};" \
);

/**
 * Restores the old SP in order to return to RedBoot.
 */
#define KERNEL_EXIT() asm( \
	"ldmed sp, {sp};" \
);


int kernel_stack_base = KERNEL_STACK_BASE;
int user_stack_base = USER_STACK_BASE;

/**
 * Handles the SWI interrupt.
 */
void swi_handler(void) {
	bwprintf(COM2, "SWI HANDLER\r\n");
  bwprintf(COM2, "lr: ");
  asm("mov r7, lr;");
  PRINT_REG("r7");
  bwprintf(COM2, "\r\n");

  bwprintf(COM2, "kstack: ");
  PRINT_REG("sp");
  bwprintf(COM2, "\r\n");
  // get arguments of request, save to stack
  // get the lr which is the pc of the user task
  
	
  // bwprintf(COM2, "  PUSHING LR TO KERNEL STACK\r\n");
  // push lr to kernel stack
  /*
  asm(
    "stmed sp!, {r14};"
  );
  */


  bwprintf(COM2, "  SWITCHING TO SYS MODE\r\n");
  // switch to sys mode
	asm(
		"mrs r0, cpsr;"
		"bic r0, r0, #31;"
		"orr r0, r0, #31;"
		"msr cpsr, r0;"
	);
  // PRINT_REG("sp");

  bwprintf(COM2, "  PUSHING TASK REGISTERS TO TASK STACK\r\n");
  // push task registers to task stack
  asm(
    "stmed sp, {r0-r15};"
    "sub sp, sp, #68;"
  );


  bwprintf(COM2, "  SWITCHING BACK TO KERNEL MODE\r\n");
  // switch back to kernel mode
	asm(
		"mrs r0, cpsr;"
		"bic r0, r0, #31;"
		"orr r0, r0, #19;"
		"msr cpsr, r0;"
	);
  bwprintf(COM2, "kstack: ");
  PRINT_REG("sp");
  bwprintf(COM2, "\r\n");

  bwprintf(COM2, "  LOADING KERNEL TRAP FRAME\r\n");
  asm(
	  "ldmed sp!, {r0-r12};"
  );
  bwprintf(COM2, "kstack: ");
  PRINT_REG("sp");
  bwprintf(COM2, "\r\n");
  bwprintf(COM2, "r8: ");
  PRINT_REG("r8");
  bwprintf(COM2, "\r\n");

  bwprintf(COM2, "lr: ");
  asm("mov r7, lr;");
  PRINT_REG("r7");
  bwprintf(COM2, "\r\n");

  bwprintf(COM2, "END SWI HANDLER\r\n");

  asm("mov pc, lr");
}


void initialize() {
  bwprintf(COM2, "Initializing...\r\n");

  // Set the SWI handler to swi_handler
	int *kep = (int *)KERNEL_ENTRY;
  *kep = (int)(&swi_handler+24); // TODO there has to be a better way...
}


void test_user_task() {
}

void start_user_task() {
  // PRINT_PSR("cpsr");
 
  asm("mov r8, #105");
  bwprintf(COM2, "r8: ");
  PRINT_REG("r8");
  bwprintf(COM2, "\r\n");
  // save kernel registers to kernel stack
  bwprintf(COM2, "kstack: ");
  PRINT_REG("sp");
  bwprintf(COM2, "\r\n");
  asm(
    "stmed sp!, {r0-r12};"
  );
  bwprintf(COM2, "kstack: ");
  PRINT_REG("sp");
  bwprintf(COM2, "\r\n");

  // set the CPSR to system mode
	asm(
		"mrs r0, cpsr;"
		"bic r0, r0, #31;"
		"orr r0, r0, #31;"
		"msr cpsr, r0;"
	);
  
  bwprintf(COM2, "user stack: ");
  PRINT_REG("sp");
  bwprintf(COM2, "\r\n");
 

  // set the sp of the task
  // TODO this has to be dynamically done
  asm(
    ".extern user_stack_base;"
	  "ldr r1, =user_stack_base;"
	  "ldr r1, [r1];"
	  "mov sp, r1;"
  );
  
  bwprintf(COM2, "user stack: ");
  PRINT_REG("sp");
  bwprintf(COM2, "\r\n");
  
  // set the CPSR to user mode
	asm(
		"mrs r0, cpsr;"
		"bic r0, r0, #31;"
		"orr r0, r0, #16;"
		"msr cpsr, r0;"
	);
  bwprintf(COM2, "user stack: ");
  PRINT_REG("sp");
  bwprintf(COM2, "\r\n");


  // TODO: load task pc
  bwprintf(COM2, "TASK1 start\r\n");
  asm("mov r7, pc;");
  PRINT_REG("r7");
	asm("swi 0x0;");
  bwprintf(COM2, "TASK1 end\r\n");
  bwprintf(COM2, "TASK1 end\r\n");
}


int main(void) {
  KERNEL_INIT();

	initialize();

  start_user_task();

  KERNEL_EXIT();
	return 0;
}
