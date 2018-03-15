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
  // Set the IRQ Stack Base
  SET_CPSR(IRQ_MODE);
  WRITE_SP(IRQ_STACK_BASE);
  SET_CPSR(KERNEL_MODE);

  // Enable Hardware Interrupts
  *(int *)(VIC1_BASE + VIC_INTENABLE_OFFSET) = VIC1_ENABLED;
  *(int *)(VIC2_BASE + VIC_INTENABLE_OFFSET) = VIC2_ENABLED;
}

void interrupt_cleanup(){
  // Disable Hardware Interrupts
  *(int *)(VIC1_BASE + VIC_INTENCLEAR_OFFSET) = VIC1_ENABLED;
  *(int *)(VIC2_BASE + VIC_INTENCLEAR_OFFSET) = VIC2_ENABLED;
}

int calc_mem_usage() {
  char c[10000];
  uint32_t sp;
  sp = 0;
  asm("mov %0, sp;":"=r"(sp));
  return (KERNEL_STACK_BASE - sp) / KERNEL_STACK_BASE;
}


void memcopy(void *d, void *s, int len) {
  int i;
  for (i = 0; i < len; ++i) {
    *((char *)(d+i)) = *((char *)(s+i));
  }
}

