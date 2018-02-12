#include <task/interrupt_matrix.h>

void im_init(interrupt_matrix *im){
  im_cb_init(&(im->TC1UI));
  im_cb_init(&(im->TC3UI));
}

int im_push(interrupt_matrix *im, TaskDescriptor * td, InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
      return im_cb_push(&(im->TC1UI), td);
			break;
		case IE_TC3UI:
      return im_cb_push(&(im->TC3UI), td);
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
			break;
	}
	return -1;
}

int im_pop(interrupt_matrix *im, InterruptEvent ie, TaskDescriptor **td){
  int r;
	switch(ie){
		case IE_TC1UI:
			r = im_cb_pop(&(im->TC1UI), td);
      KASSERT(r == 0);
      return r;
			break;
		case IE_TC3UI:
			r = im_cb_pop(&(im->TC3UI), td);
      KASSERT(r == 0);
      return r;
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
			break;
	}
	return -1;	
}

int im_eventsize(interrupt_matrix *im, InterruptEvent ie){
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
