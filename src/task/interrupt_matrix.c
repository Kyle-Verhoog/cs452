#include <task/interrupt_matrix.h>

void im_init(interrupt_matrix *im){
	init_circularBuffer(&(im->TC1UI), im->buffer_TC1UI, IRQ_MAX_SIZE);
	init_circularBuffer(&(im->TC3UI), im->buffer_TC3UI, IRQ_MAX_SIZE);
}

int im_push(interrupt_matrix *im, TaskDescriptor * td, InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
			return push_circularBuffer(&(im->TC1UI), (unsigned int)td);
			break;
		case IE_TC3UI:
			return push_circularBuffer(&(im->TC3UI), (unsigned int)td);
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
			break;
	}
	return -1;
}

TaskDescriptor * im_top(interrupt_matrix *im, InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
			return (TaskDescriptor *)top_circularBuffer(&(im->TC1UI));
			break;
		case IE_TC3UI:
			return (TaskDescriptor *)top_circularBuffer(&(im->TC3UI));
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
			break;
	}
	return NULL;	
}

int im_pop(interrupt_matrix *im, InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
			return pop_circularBuffer(&(im->TC1UI));
			break;
		case IE_TC3UI:
			return pop_circularBuffer(&(im->TC3UI));
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
			break;
	}
	return -1;	
}

unsigned int im_eventsize(interrupt_matrix *im, InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
			return (im->TC1UI).size;
			break;
		case IE_TC3UI:
			return (im->TC3UI).size;
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
			break;
	}
	return -1;
}
