#include <interrupt.h>

#define WAKE_PRIORITY_SIZE 2

InterruptEvent WakePriority[64] = {IE_TC1UI, IE_TC3UI};

int get_interrupt_ret(InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
			return *(int *)(TIMER1_BASE | VAL_OFFSET);
			break;
		case IE_TC3UI:
			return *(int *)(TIMER3_BASE | VAL_OFFSET);
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
	}

	return -1;
}

void wakeall(interrupt_matrix *im, InterruptEvent ie){
	while(im_eventsize(im, ie) > 0){
		TaskDescriptor *td = im_top(im, ie);
		td->ret = get_interrupt_ret(ie);
		pq_push(&pq_tasks, td->priority, td);
		im_pop(im, ie);
	}
}

void clear_interrupt(InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
			*(int *)(TIMER1_BASE | CLR_OFFSET) = 1;
			break;
		case IE_TC3UI:
			*(int *)(TIMER3_BASE | CLR_OFFSET) = 1;
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
	}
}

bool is_interrupt_asserted(InterruptEvent bit){
	int IRQstatus;

	if(bit < 32){
		IRQstatus = *(int *)(VIC1_BASE | VIC_IRQSTATUS_OFFSET);
	}else{
		IRQstatus = *(int *)(VIC2_BASE | VIC_IRQSTATUS_OFFSET);
		bit -= 32;
	}

	return (IRQstatus >> bit) & 1;
}

void init_irq(interrupt_matrix *im){
  //Initialize Interrupt Matrix
  im_init(im);

  //Set the IRQ Stack Base
  SET_CPSR(IRQ_MODE);
  WRITE_SP(IRQ_STACK_BASE);
  SET_CPSR(KERNEL_MODE);

  //Enable Hardware Interrupts
  *(int *)(VIC1_BASE + VIC_INTENABLE_OFFSET) = VIC1_ENABLED;
  *(int *)(VIC2_BASE + VIC_INTENABLE_OFFSET) = VIC2_ENABLED;
}

void cleanup_irq(){
	//Disable Hardware Interrupts
  *(int *)(VIC1_BASE + VIC_INTENCLEAR_OFFSET) = VIC1_ENABLED;
  *(int *)(VIC2_BASE + VIC_INTENCLEAR_OFFSET) = VIC2_ENABLED;
}

void event_register(interrupt_matrix *im, TaskDescriptor *td){
	//Get the eventId from r0 user stack
	volatile InterruptEvent eventid;
	asm("ldr %0, [%1, #4];":"=r"(eventid):"r"(td->sp));

	//Add waiting task to matrix
	im_push(im, td, eventid);
}

void event_wake(interrupt_matrix *im){
    int i;
    for( i = 0; i < WAKE_PRIORITY_SIZE; ++i){
    	if(is_interrupt_asserted(WakePriority[i])){
    		clear_interrupt(WakePriority[i]);
    		if(im_eventsize(im, WakePriority[i]) > 0){
    			wakeall(im, WakePriority[i]);
    			return;
    		}
    	}
    }
}
