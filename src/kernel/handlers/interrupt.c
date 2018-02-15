#include <kernel/handlers/interrupt.h>

#define WAKE_PRIORITY_SIZE 6

InterruptEvent WakePriority[64] = {IE_TC1UI, 
								IE_TC3UI, 
								IE_UART2_TX, 
								IE_UART2_RX, 
								IE_UART2_RT, 
								IE_UART2_MI};

int get_interrupt_ret(InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
			return *(int *)(TIMER1_BASE | VAL_OFFSET);
		case IE_TC3UI:
			return *(int *)(TIMER3_BASE | VAL_OFFSET);
		case IE_UART2_TX:
			return 0;
		case IE_UART2_RX:
			return 0;
		case IE_UART2_MI:
			return 0;
		case IE_UART2_RT:
			return 0;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
	}

	return -1;
}

void remove_assert(interrupt_matrix *im, InterruptEvent ie){
	im->stored_flag[ie] = 0;
}

void store_assert(interrupt_matrix *im, InterruptEvent ie){
	im->stored_flag[ie]++;
}

void wakeall(interrupt_matrix *im, InterruptEvent ie){
	int r;
	while(im_eventsize(im, ie) > 0){
		TaskDescriptor *td;
	    r = im_pop(im, ie, &td);
	    KASSERT(r == 0);
		td->ret = get_interrupt_ret(ie);
		pq_push(&pq_tasks, td->priority, td);
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
		case IE_UART2_TX:
			*(int *)(UART2_BASE + UART_CTRL_OFFSET) &= ~(1 << 5);
			break;
		case IE_UART2_RX:
			*(int *)(UART2_BASE + UART_CTRL_OFFSET) &= ~(1 << 4);
			break;
		case IE_UART2_RT:
			*(int *)(UART2_BASE + UART_CTRL_OFFSET) &= ~(1 << 6);
			break;
		case IE_UART2_MI:
			*(int *)(UART2_BASE + UART_CTRL_OFFSET) &= ~(1 << 3);
			*(int *)(UART2_BASE | UART_INTR_OFFSET) = 1;
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
	}
}

bool is_interrupt_stored(interrupt_matrix *im, InterruptEvent ie){
	return im->stored_flag[ie];
}

bool is_interrupt_asserted(InterruptEvent bit){
	int IRQstatus;
	int result = 0;

	if(bit < 32){
		IRQstatus = *(int *)(VIC1_BASE | VIC_IRQSTATUS_OFFSET);
		result = (IRQstatus >> bit) & 1;
	}else if(bit < 64){
		bit -= 32;
		IRQstatus = *(int *)(VIC2_BASE | VIC_IRQSTATUS_OFFSET);
		result = (IRQstatus >> bit) & 1;
	}else{
		switch(bit){
			case IE_UART2_TX:
				bit = IE_UART2 - 32;
				IRQstatus = *(int *)(VIC2_BASE | VIC_IRQSTATUS_OFFSET);
				result = ((IRQstatus >> bit) & 1) && *(int *)(UART2_BASE | UART_INTR_OFFSET) & 0x4;			
				break;
			case IE_UART2_RX:
				bit = IE_UART2 - 32;
				IRQstatus = *(int *)(VIC2_BASE | VIC_IRQSTATUS_OFFSET);
				result = ((IRQstatus >> bit) & 1) && *(int *)(UART2_BASE | UART_INTR_OFFSET) & 0x2;
				break;
			case IE_UART2_RT:
				bit = IE_UART2 - 32;
				IRQstatus = *(int *)(VIC2_BASE | VIC_IRQSTATUS_OFFSET);
				result = ((IRQstatus >> bit) & 1) && *(int *)(UART2_BASE | UART_INTR_OFFSET) & 0x8;
				break;
			case IE_UART2_MI:
				bit = IE_UART2 - 32;
				IRQstatus = *(int *)(VIC2_BASE | VIC_IRQSTATUS_OFFSET);
				result = ((IRQstatus >> bit) & 1) && *(int *)(UART2_BASE | UART_INTR_OFFSET) & 0x1;
				break;
			default:
				KASSERT(0 && "Bad InterruptEvent Specified.");
				break;
		}
	}
	return result;
}

void init_irq(interrupt_matrix *im){
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
	volatile InterruptEvent ie;
	asm("ldr %0, [%1, #4];":"=r"(ie):"r"(td->sp));

  if (is_interrupt_stored(im, ie)) {
    pq_push(&pq_tasks, td->priority, td);
    remove_assert(im, ie);
  } else {
    //Add waiting task to matrix
    im_push(im, td, ie);
  }
}

void event_wake(interrupt_matrix *im){
  int i;
  InterruptEvent ie;

  for( i = 0; i < WAKE_PRIORITY_SIZE; ++i){
    ie = WakePriority[i];
    if(is_interrupt_asserted(ie) ||
        is_interrupt_stored(im, ie)){
      if(im_eventsize(im, ie) > 0){
        wakeall(im, ie);
        clear_interrupt(ie);
        remove_assert(im, ie);
        return;
      }else{
        store_assert(im, ie);
        clear_interrupt(ie);
      }
    }
  }
}
