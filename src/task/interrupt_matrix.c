#include <task/interrupt_matrix.h>

void im_init(interrupt_matrix *im){
	init_circularBuffer(&(im->TC3UI), im->buffer_TC3UI, 256); //TODO: MAKE DEFINE
}

int im_push(interrupt_matrix *im, TaskDescriptor * td, InterruptEvent ie){
	switch(ie){
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
		case IE_TC3UI:
			return (im->TC3UI).size;
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
			break;
	}
	return -1;
}

