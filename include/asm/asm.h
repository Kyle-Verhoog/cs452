#ifndef ASM_H
#define ASM_H

#define asm __asm__ volatile

//Macros
#define PRINT_REG(reg)\
  asm(\
      "mov  r0, #1;"   \
      "mov  r1, "reg";"\
      "bl   bwputr;" \
     );

#define PRINT_PSR(psr) \
  asm(\
      "mov  r0, #1;"   \
      "mrs  r1, "psr";"\
      "bl   bwputr;" \
     );

#define PUSH_STACK(param) \
  asm( \
       "stmfd sp!, {"param"};" \
     );

#define POP_STACK(param) \
  asm( \
       "ldmfd sp!, {"param"};" \
     );

#define WRITE_SP(val) \
  asm( \
       "mov sp, %0;"::"r"(val) \
     );

#define WRITE_SPSR(val) \
  asm( \
       "msr spsr, %0;"::"r"(val) \
     );

#define READ_SP(var) \
  asm( \
       "mov %0, sp;":"=r"(var): \
     );

#define READ_SPSR(var) \
  asm( \
       "mrs %0, spsr;":"=r"(var): \
     );


#define SET_CPSR(mode) \
  asm( \
       "mrs r12, cpsr;" \
       "bic r12, r12, #31;" \
       "orr r12, r12, #"STR(mode)";" \
       "msr cpsr, r12;" \
     );

#define SWI(arg) \
  asm( \
       "swi %0;"::"r"(arg) \
     );

#define REVERSE_SWI() \
  asm( \
       "movs pc, lr" \
     )

#define SWI_ARG_FETCH(reg) \
  asm( \
       "ldr "reg", [lr, #-4];" \
       "bic "reg", "reg", #0xff000000;" \
     );

//COPIED FROM ARM INFO CENTER
#define ENABLE_ALL_CACHE() \
  asm( \
       "mov r0, #0;" \
       "mov r1, #0;" \
       "MRC p15, 0, r1, c1, c0, 0;" \
       "ORR r1, r1, #0x1 <<12;" \
       "ORR r1, r1, #0x1 <<2;" \
       "MCR p15, 0, r0, c15, c5, 0;" \
       "MCR p15, 0, r0, c7, c5, 0;" \
       "MCR p15, 0, r1, c1, c0, 0;" \
     );

//COPIED FROM ARM INFO CENTER
#define DISABLE_ALL_CACHE() \
  asm( \
       "MRC p15, 0, r1, c1, c0, 0;" \
       "BIC r1, r1, #0x1 <<12;" \
       "MCR p15, 0, r1, c1, c0, 0;" \
     );

#endif /* ASM_H */
