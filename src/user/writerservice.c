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

	*ptr++ = '\033';
	*ptr++ = '[';
	ui2a(r, 10, &rsize, ptr);
	ptr += rsize;
	*ptr++ = ';';
	ui2a(c, 10, &csize, ptr);
	ptr+=csize;
	*ptr++ = 'H';

  PutStr(tx_tid, command, rsize+csize+4);
}

void PushCharToUART2(tid_t tx_tid, WRProtocol *wrp, Cursor *cursor){
	char c = *wrp->data;
  char str[5];
  char *ptr;
  ptr = str;

	switch(c){
		case BACKSPACE:
			cursor->col -= 1;
      *ptr++ = c;
      *ptr++ = ' ';
      *ptr++ = c;
      PutStr(tx_tid, str, 3);
			break;
		case CARRIAGE_RETURN:
			cursor->col = 1;
			cursor->row += 1;
      *ptr++ = '\n';
      *ptr++ = '\r';
      PutStr(tx_tid, str, 2);
			break;
		default:
			cursor->col += 1;
			PutC(tx_tid, c);
			break;
	}
}

void PushCommandToUART1(tid_t tx_tid, WRProtocol *wrp){
  PutStr(tx_tid, wrp->data, wrp->size);
}

void PushCommandToUART2(tid_t tx_tid, WRProtocol *wrp, Cursor *original){
	//Push command to ioserver
	move_cursor(tx_tid, wrp->cursor.row, wrp->cursor.col);
  PutStr(tx_tid, wrp->data, wrp->size);
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
