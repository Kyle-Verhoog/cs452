#include <system.h>

/**
 * Prints out an error message and aborts the kernel.
 */
void kpanic(const char *fmt, ...) {
  /*
  va_list va;

  va_start(va,fmt);
  bwformat(LOG_COM, fmt, va);
  va_end(va);
  */
  KABORT();
}

void interrupt_init() {
  //Set the IRQ Stack Base
  SET_CPSR(IRQ_MODE);
  WRITE_SP(IRQ_STACK_BASE);
  SET_CPSR(KERNEL_MODE);

  //Enable Hardware Interrupts
  *(int *)(VIC1_BASE + VIC_INTENABLE_OFFSET) = VIC1_ENABLED;
  *(int *)(VIC2_BASE + VIC_INTENABLE_OFFSET) = VIC2_ENABLED;
}

void interrupt_cleanup(){
	//Disable Hardware Interrupts
  *(int *)(VIC1_BASE + VIC_INTENCLEAR_OFFSET) = VIC1_ENABLED;
  *(int *)(VIC2_BASE + VIC_INTENCLEAR_OFFSET) = VIC2_ENABLED;
}
