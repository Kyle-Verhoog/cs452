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
	bwprintf(COM2, "SWI HANDLER\n\r");
}


void initialize() {
	//Move our stack pointer to base of kernel
	PRINT_REG("sp");

  // Set the SWI handler to swi_handler
	int *kep = (int *)KERNEL_ENTRY;
  *kep = (int)&swi_handler;
}


int main( int argc, char* argv[] ) {
	PRINT_REG("sp");
  KERNEL_INIT();
	//Set the extern
	//Move stack pointer to new kernel stack
	
  //Store previous stack pointer

	PRINT_REG("sp");

	// initialize();

	// asm(
	// 	"mrs r0, cpsr;"
	// 	"bic r0, r0, #0x3 << 6;"
	// 	"msr cpsr, r0;"
	// 	);

	//PRINT_REG("sp");

	// asm(
	//  "mov  r0, #1;"
	//  "mrs  r1, cpsr;"
	//  "bl   bwputr;"
	// );

	// //Set the CPSR to user mode
	// asm(
	// 	"mrs r0, cpsr;"
	// 	"bic r0, r0, #31;"
	// 	"orr r0, r0, #16;"
	// 	"msr cpsr, r0;"
	// );

	// asm(
	// 	"mrs r0, cpsr;"
	// 	"bic r0, r0, #31;"
	// 	"orr r0, r0, #19;"
	// 	"msr cpsr, r0;"
	// );

	// // asm(
	// //   "swi 0x0;"
	// // );

	// asm(
	//  "mov  r0, #1;"
	//  "mrs  r1, cpsr;"
	//  "bl   bwputr;"
	// );

  KERNEL_EXIT();
	PRINT_REG("sp");
	return 0;
}
