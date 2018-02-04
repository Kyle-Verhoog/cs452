#include <interrupt_matrix.h>

void im_init(interrupt_matrix *im){
	init_circularBuffer(&(im->TC3UI));
}

int im_push(interrupt_matrix *im, TaskDescriptor * td, InterruptEvent ie){
	switch(ie){
		case IE_TC3UI:
			return push_circularBuffer(&(im->TC3UI), (unsigned int)td);
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
	}
	return 0;
}

TaskDescriptor * im_top(interrupt_matrix *im, InterruptEvent ie){
	switch(ie){
		case IE_TC3UI:
			return (TaskDescriptor *)top_circularBuffer(&(im->TC3UI));
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
	}
	return 0;	
}

int im_pop(interrupt_matrix *im, InterruptEvent ie){
	switch(ie){
		case IE_TC3UI:
			return pop_circularBuffer(&(im->TC3UI));
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
	}
	return 0;	
}

unsigned int im_eventsize(interrupt_matrix *im, InterruptEvent ie){
	switch(ie){
		case IE_TC3UI:
			return (im->TC3UI).size;
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
	}
	return 0;
}