#include <kernel.h>

#define asm __asm__

void handler(void) {
  // TODO
	bwprintf(COM2, "BAD ACCESS");
}


void initialize() {
  int *kep = (int *)KERNEL_ENTRY;
  *kep = &handler;

  int *kep4 = (int *)(KERNEL_ENTRY + 4);
  *kep4 = &handler;

	int *kep8 = (int *)(KERNEL_ENTRY + 8);
  *kep8 = &handler;

  *(int *)(KERNEL_ENTRY + 12) = &handler;
  *(int *)(KERNEL_ENTRY + 16) = &handler;
  *(int *)(KERNEL_ENTRY + 20) = &handler;
  *(int *)(KERNEL_ENTRY + 24) = &handler;
  *(int *)(KERNEL_ENTRY + 28) = &handler;
  *(int *)(KERNEL_ENTRY + 32) = &handler;
  *(int *)(KERNEL_ENTRY + 36) = &handler;
  *(int *)(KERNEL_ENTRY + 40) = &handler;

  *(int *)(KERNEL_ENTRY - 4) = &handler;
}

int main( int argc, char* argv[] ) {

  initialize();

  // asm(
  // 	"mrs r0, cpsr;"
  // 	"bic r0, r0, #0x3 << 6;"
  // 	"msr cpsr, r0;"
  // 	);
  bwprintf(COM2, "%x\n\r", *(int*)(0x10));
  bwprintf(COM2, "%x\n\r", *(int*)(0x8));

  asm(
   "mov  r0, #1;"
   "mrs  r1, cpsr;"
   "bl   bwputr;"
  );

  //Set the CPSR to user mode
  asm(
  	"mrs r0, cpsr;"
  	"bic r0, r0, #31;"
  	"orr r0, r0, #16;"
  	"msr cpsr, r0;"
  );

  asm(
  	"mrs r0, cpsr;"
  	"bic r0, r0, #31;"
  	"orr r0, r0, #19;"
  	"msr cpsr, r0;"
  );

  // asm(
  //   "swi 0x0;"
  // );

  asm(
   "mov  r0, #1;"
   "mrs  r1, cpsr;"
   "bl   bwputr;"
  );

	return 0;
}
