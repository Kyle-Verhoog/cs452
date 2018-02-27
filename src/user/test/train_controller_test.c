#include<train_controller_test.h>

int tryputc ( int channel, char c )
{
	int *flags, *data;
	switch( channel ){
		case COM1:
			flags = (int *)(UART1_BASE | UART_FLAG_OFFSET);
			data = (int *)(UART1_BASE | UART_DATA_OFFSET);
			break;
		case COM2:
			flags = (int *)(UART2_BASE | UART_FLAG_OFFSET);
			data = (int *)(UART2_BASE | UART_DATA_OFFSET);
			break;
		default:
			return -1;
			break;
	}

	if(!(*flags & TXFF_MASK)){
		if(channel == COM1 && (*flags & CTS_MASK)){
			*data = c;
			return 1;	
		}
		else if(channel == COM2){
			*data = c;
			return 1;	
		}
	}

	return 0;
}


void ControllerMetricTest(){
		//Disable Hardware Interrupts
  *(int *)(VIC1_BASE + VIC_INTENCLEAR_OFFSET) = VIC1_ENABLED;
  *(int *)(VIC2_BASE + VIC_INTENCLEAR_OFFSET) = VIC2_ENABLED;
	
#ifdef TASK_METRICS
	int metrics[5];
	int m = 0;

	//go
	char *command = "`````````````````````````````````````````````````````````````````````````````````````````````````````";
	//sensor
	char *command2 = "\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85\x85";
	//speed tr (14, 24)
	char *command3 = "\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18\xe\x18";
	int size = 101;
	int size2 = 101;
	int size3 = 202;
	int ts, te;

	//SET THE 2 STOP BIT
	*(int*)(UART1_BASE | UART_LCRH_OFFSET) = *(int*)(UART1_BASE | UART_LCRH_OFFSET) | STP2_MASK;
	//SET SPEED PROPERLY
	*(int *)( UART1_BASE + UART_LCRL_OFFSET ) = 0xBF;

	PRINTF("TESTING GO\n\r");
	ts = *(int *)TM_CLOCK_VAL;
	while(size > 0){
		if(tryputc(COM1, command[size - 1])){
			size--;
		}
	}
	te = *(int *)TM_CLOCK_VAL;
	metrics[0] = ts - te;

	PRINTF("TESTING SENSOR QUERY\n\r");
	ts = *(int *)TM_CLOCK_VAL;
	while(size2 > 0){
		if(tryputc(COM1, command2[size2 - 1])){
			size2--;
		}
	}
	te = *(int *)TM_CLOCK_VAL;
	metrics[1] = ts - te;
	
	PRINTF("TESTING MOVE QUERY\n\r");
	ts = *(int *)TM_CLOCK_VAL;
	while(size3 > 0){
		if(tryputc(COM1, command3[size3 - 1])){
			size3--;
			while(true){
				if(tryputc(COM1, command3[size3 - 1])){
					size3--;
					break;
				}
			}
		}
	}
	te = *(int *)TM_CLOCK_VAL;
	metrics[2] = ts - te;
	
	PRINTF("GO PROCESS TIME: %dms\n\r", metrics[0]/508);
	PRINTF("SENSOR QUERY PROCESS TIME: %dms\n\r", metrics[1]/508);
	PRINTF("MOVE TRAIN PROCESS TIME: %dms\n\r", metrics[2]/508);
#endif TASK_METRICS

	  //Enable Hardware Interrupts
  *(int *)(VIC1_BASE + VIC_INTENABLE_OFFSET) = VIC1_ENABLED;
  *(int *)(VIC2_BASE + VIC_INTENABLE_OFFSET) = VIC2_ENABLED;

	COMPLETE_TEST();
}
