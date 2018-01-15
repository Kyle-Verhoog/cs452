#include <kernel.h>

#define asm __asm__


void handler(void) {
  // TODO
}


void initialize() {
  int *kep = (int *)KERNEL_ENTRY;
  *kep = &handler;
}

int main( int argc, char* argv[] ) {

  initialize();

  asm(
    "swi 0x0;"
  );

	return 0;
}
