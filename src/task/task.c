#include <task.h>
#include <ts7200.h>
#include <bwio.h>

void taskOne(){
	int counter = 0;
	bwprintf(COM2, "PRINT %d\n\r", counter);
	counter++;

	asm(
		"swi #0;"
	);
	
	bwprintf(COM2, "PRINT %d\n\r", counter);
	counter++;

	asm(
		"swi #0;"
	);

	bwprintf(COM2, "PRINT %d\n\r", counter);
	bwprintf(COM2, "PRINT %d AGAIN\n\r", counter);
	counter++;

	asm(
		"swi #0;"
	);	
}
