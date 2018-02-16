#include<writerservice.h>

void WriteCommandUART1(tid_t writer, char *command, int size){
	int reply;
	WRProtocol wrp;
	wrp.wr_req = WR_COMMAND;
	wrp.data = command;
	wrp.size = size;

	Send(writer, &wrp, sizeof(wrp), &reply, sizeof(reply));
}

void WriteCharUART2(tid_t writer, char c){
	int reply;
	WRProtocol wrp;
	wrp.wr_req = WR_CHAR;
	wrp.data = &c;
	wrp.size = 1;

	Send(writer, &wrp, sizeof(wrp), &reply, sizeof(reply));
}

void WriteCommandUART2(tid_t writer, char *command, int size, Cursor *cursor){
	int reply;
	WRProtocol wrp;
	wrp.wr_req = WR_COMMAND;
	wrp.data = command;
	wrp.size = size;
	wrp.cursor.row = cursor->row;
	wrp.cursor.col = cursor->col;

	Send(writer, &wrp, sizeof(wrp), &reply, sizeof(reply));
}

void WriteStringUART2(tid_t writer, char *command, Cursor *cursor){
	int reply;
	WRProtocol wrp;
	wrp.wr_req = WR_COMMAND;
	wrp.data = command;
	wrp.size = 0;
	wrp.cursor.row = cursor->row;
	wrp.cursor.col = cursor->col;

	while(true){
		if(command[wrp.size] == '\0'){
			break;
		}
		wrp.size++;
	}

	Send(writer, &wrp, sizeof(wrp), &reply, sizeof(reply));
}

void move_cursor(tid_t tx_tid, int r, int c){
	char command[COMMAND_SIZE];
	char *ptr = command;
	int rsize;
	int csize;

	*ptr++ = '\0';
	*ptr++ = '3';
	*ptr++ = '3';
	*ptr++ = '[';
	ui2a(r, 10, &rsize, ptr);
	ptr += rsize;
	*ptr++ = ';';
	ui2a(c, 10, &csize, ptr);
	ptr+=csize;
	*ptr++ = 'H';

	// int i;
	// for(i = 0; i < rsize + csize + 6; i++){
	// 	PutC(tx_tid, command[i]);
	// }
}

void PushCharToUART2(tid_t tx_tid, WRProtocol *wrp, Cursor *cursor){
	char c = *wrp->data;

	switch(c){
		case BACKSPACE:
			cursor->col -= 1;
			PutC(tx_tid, c);
			PutC(tx_tid, ' ');
			PutC(tx_tid, c);
			break;
		case CARRIAGE_RETURN:
			cursor->col = 1;
			cursor->row += 1;
			PutC(tx_tid, '\n');	//TODO: Perhaps clear line instead
			PutC(tx_tid, '\r');
			break;
		default:
			cursor->col += 1;
			PutC(tx_tid, c);
			break;
	}
}

void PushCommandToUART1(tid_t tx_tid, WRProtocol *wrp){
	int i;
	for(i = 0; i < wrp->size; i++){
		PutC(tx_tid, wrp->data[i]);
	}
}

void PushCommandToUART2(tid_t tx_tid, WRProtocol *wrp, Cursor *original){
	//Push command to ioserver
	move_cursor(tx_tid, wrp->cursor.row, wrp->cursor.col);
	int i;
	for(i = 0; i < wrp->size; i++){
		PutC(tx_tid, wrp->data[i]);
	}
	move_cursor(tx_tid, original->row, original->col);
}

void WriterServiceUART1(){
	int reply = 0;
	int r = RegisterAs(WRITERSERVICE_UART1_ID);
	assert(r == 0);

	tid_t tx_tid = WhoIs(IOSERVER_UART1_TX_ID);
	assert(tx_tid >= 0);

	while(true){
		tid_t req_tid;
		WRProtocol wrp;
		
		//Receive a request
		Receive(&req_tid, &wrp, sizeof(wrp));

		//Handle Request
		switch(wrp.wr_req){
			case WR_COMMAND:
				PushCommandToUART1(tx_tid, &wrp);
				Reply(req_tid, &reply, sizeof(reply));
				break;
			default:
				assert(0 && "Bad Request");
				break;
		}
	}

	Exit();
}

void WriterServiceUART2(){
	int reply = 0;

	int r = RegisterAs(WRITERSERVICE_UART2_ID);
	assert(r == 0);

	tid_t tx_tid = WhoIs(IOSERVER_UART2_TX_ID);
	assert(tx_tid >= 0);

	Cursor cursor;
	cursor.row = 1;
	cursor.col = 1;

	while(true){
		tid_t req_tid;
		WRProtocol wrp;

		//Receive a request
		Receive(&req_tid, &wrp, sizeof(wrp));

		//Handle Request
		switch(wrp.wr_req){
			case WR_CHAR:
				PushCharToUART2(tx_tid, &wrp, &cursor);
				Reply(req_tid, &reply, sizeof(reply));
				break;
			case WR_COMMAND:
				PushCommandToUART2(tx_tid, &wrp, &cursor);
				Reply(req_tid, &reply, sizeof(reply));
				break;
			default:
				assert(0 && "Bad request");
				break;
		}
	}

	Exit();
}
