#include <task/interrupt_matrix.h>

void im_init(interrupt_matrix *im){
  im_cb_init(&(im->TC1UI));
  im_cb_init(&(im->TC3UI));
  im_cb_init(&(im->UART2_RX));
  im_cb_init(&(im->UART2_TX));
  im_cb_init(&(im->UART2_RT));
  im_cb_init(&(im->UART2_MI));

  int i;
  for(i = 0; i < INTERRUPT_SIZE; i++ ){
  	im->stored_flag[i] = 0;
  }
}

int im_push(interrupt_matrix *im, TaskDescriptor * td, InterruptEvent ie){
	switch(ie){
		case IE_TC1UI:
      		return im_cb_push(&(im->TC1UI), td);
			break;
		case IE_TC3UI:
      		return im_cb_push(&(im->TC3UI), td);
			break;
		case IE_UART2_RX:
	  		return im_cb_push(&(im->UART2_RX), td);
			break;
		case IE_UART2_TX:
	  		return im_cb_push(&(im->UART2_TX), td);
			break;
		case IE_UART2_RT:
	  		return im_cb_push(&(im->UART2_RT), td);
			break;
		case IE_UART2_MI:
	  		return im_cb_push(&(im->UART2_MI), td);
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
		case IE_UART2_RX:
			r = im_cb_pop(&(im->UART2_RX), td);
		    KASSERT(r == 0);
		    return r;
		case IE_UART2_TX:
			r = im_cb_pop(&(im->UART2_TX), td);
		    KASSERT(r == 0);
		    return r;	
		case IE_UART2_RT:
			r = im_cb_pop(&(im->UART2_RT), td);
		    KASSERT(r == 0);
		    return r;	    
		case IE_UART2_MI:
			r = im_cb_pop(&(im->UART2_MI), td);
		    KASSERT(r == 0);
		    return r;
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
		case IE_UART2_RX:
			return (im->UART2_RX).size;
			break;			
		case IE_UART2_TX:
			return (im->UART2_TX).size;
			break;
		case IE_UART2_RT:
			return (im->UART2_RT).size;
			break;
		case IE_UART2_MI:
			return (im->UART2_MI).size;
			break;
		default:
			KASSERT(0 && "Bad InterruptEvent Specified.");
			break;
	}
	return -1;
}
