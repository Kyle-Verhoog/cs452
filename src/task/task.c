#include <task.h>
#include <ts7200.h>
#include <bwio.h>

void taskOne(){
	bwprintf(COM2, "PRINT ONE\n\r");

	asm(
		"swi #0;"
	);
	
	bwprintf(COM2, "PRINT TWO\n\r");

	asm(
		"swi #0;"
	);

	bwprintf(COM2, "PRINT THREE\n\r");
	bwprintf(COM2, "PRINT THREE AGAIN\n\r");

	asm(
		"swi #0;"
	);	
}
