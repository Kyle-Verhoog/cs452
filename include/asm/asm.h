#ifndef ASM_H
#define ASM_H

//Macros
#define PRINT_REG(reg)   asm(				\
						   "mov  r0, #1;" 	\
						   "mov  r1, "reg";"\
						   "bl   bwputr;"	\
						  );

#define PRINT_PSR(psr)   asm(				\
						   "mov  r0, #1;" 	\
						   "mrs  r1, "psr";"\
						   "bl   bwputr;"	\
						  );

#define PUSH_STACK(param)	asm( \
					    		"stmfd sp!, {"param"};" \
					  		);

#define POP_STACK(param)	asm ( \
								"ldmfd sp!, {"param"};" \
							);	

#define WRITE_SP(val)	asm( \
					    	"mov sp, %0;"::"r"(val) \
						);

#define WRITE_SPSR(val) asm( \
							"msr spsr, %0;"::"r"(val) \
						);

#define READ_SP(var) 	asm ( \
							"mov %0, sp;":"=r"(var): \
						);

#define READ_SPSR(var)	asm ( \
							"mrs %0, spsr;":"=r"(var): \
						);


#define SET_CPSR(mode)  asm( \
							"mrs r12, cpsr;" \
							"bic r12, r12, #31;" \
							"orr r12, r12, #"STR(mode)";" \
							"msr cpsr, r12;" \
						);

#define SWI(arg) asm ( \
					"swi %0;"::"r"(arg) \
				);

#define REVERSE_SWI() 	asm( \
							"movs pc, lr" \
						)

#define SWI_ARG_FETCH(reg) asm( \
						    "ldr "reg", [lr, #-4];" \
						    "bic "reg", "reg", #0xff000000;" \
						);

#endif /* ASM_H */
