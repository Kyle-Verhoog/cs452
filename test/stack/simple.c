#include <kernel.h>

#define asm __asm__

int pushing_to_stack( int argc, char* argv[] ) {
	//Kernel happens here
  bwprintf(COM2, "pc: ");
  asm(
   "mov  r0, #1;"
   "mov  r1, pc;"
   "bl   bwputr;"
  );
  bwprintf(COM2, "sp: ");
  asm(
   "mov  r0, #1;"
   "mov  r1, sp;"
   "bl   bwputr;"
  );
  
  asm(
   "mov r3, #9;"
   "str r3, [sp, #-4]!;"
  );
 
  bwprintf(COM2, "sp: ");
  asm(
   "mov  r0, #1;"
   "mov  r1, sp;"
   "bl   bwputr;"
  );
  
  asm(
   "ldr r1, [sp], #4;"
  );
  
  asm(
   "mov  r0, #1;"
   "bl   bwputr;"
  );
  
  bwprintf(COM2, "pc: ");
  asm(
   "mov  r0, #1;"
   "mov  r1, pc;"
   "bl   bwputr;"
  );
	return 0;
}
