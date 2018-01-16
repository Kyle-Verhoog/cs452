#include <kernel.h>

#define asm __asm__

int kernel_stack_base = KERNEL_STACK_BASE;
int user_stack_base = USER_STACK_BASE;

void handler(void) {
  	// TODO
	bwprintf(COM2, "BAD ACCESS");
}

void initialize() {
	//Move our stack pointer to base of kernel
	PRINT_REG("sp");

	//Set the extern
	asm(
		".extern kernel_stack_base;"
		".extern user_stack_base;"
	);

	//Move stack pointer to kernel head
	asm(
		"ldr r6, =kernel_stack_base;"
		"ldr r6, [r6];"
		"mov r5, sp;"
		"mov sp, r6;"
	);
	
	//Store previous stack pointer
	asm(
		"stmed sp!, {r5};"
	);

	PRINT_REG("sp");

	int *kep = (int *)KERNEL_ENTRY;
  	*kep = &handler;

  	PRINT_REG("sp");
}

void cleanup() {
	//Pop kernel stack and put to SP
	PRINT_REG("sp");
	asm(
		//"ldr sp, [sp, #4];"
		"ldr r7, [sp, #4];"
		);

	PRINT_REG("r7");
}

int main( int argc, char* argv[] ) {

	initialize();

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
	cleanup();

	return 0;
}
