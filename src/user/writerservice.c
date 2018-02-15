#include<writerservice.h>

void WriteChar(tid_t writer, char c){
	int reply;
	WRProtocol wrp;
	wrp.wr_req = WR_CHAR;
	wrp.data = &c;
	wrp.size = 1;

	Send(writer, &wrp, sizeof(wrp), &reply, sizeof(reply));
}

void WriteCommand(tid_t writer, char *command, int size, Cursor *cursor){
	int reply;
	WRProtocol wrp;
	wrp.wr_req = WR_COMMAND;
	wrp.data = command;
	wrp.size = size;
	wrp.cursor.row = cursor->row;
	wrp.cursor.col = cursor->col;

	Send(writer, &wrp, sizeof(wrp), &reply, sizeof(reply));
}

void move_cursor(tid_t ios_tid, int r, int c){
	char command[COMMAND_SIZE];
	char *ptr = command;
	int rsize;
	int csize;

	*ptr = '\0';
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
	// 	PutC(ios_tid, command[i]);
	// }

	*ptr++ = '\0';
	PRINTF(command);
}

void PushCharToIO(tid_t ios_tid, WRProtocol *wrp, Cursor *cursor){
	char c = *wrp->data;

	// switch(c){
	// 	case BACKSPACE:
	// 		cursor->col -= 1;
	// 		PutC(ios_tid, c);
	// 		PutC(ios_tid, ' ');
	// 		PutC(ios_tid, c);
	// 		break;
	// 	case CARRIAGE_RETURN:
	// 		cursor->col = 1;
	// 		cursor->row += 1;
	// 		PutC(ios_tid, '\n');	//TODO: Perhaps clear line instead
	// 		PutC(ios_tid, '\r');
	// 		break;
	// 	default:
	// 		cursor->col += 1;
	// 		PutC(ios_tid, c);
	// 		break;
	// }
	switch(c){
		case BACKSPACE:
			cursor->col -= 1;
			PRINTF("%c %c", BACKSPACE, BACKSPACE);
			break;
		case CARRIAGE_RETURN:
			cursor->col = 1;
			cursor->row += 1;
			PRINTF("\n\r");
			break;
		default:
			cursor->col += 1;
			PRINTF("%c", c);
			break;
	}
}

void PushCommandToIO(tid_t ios_tid, WRProtocol *wrp, Cursor *original){
	//Push command to ioserver
	// move_cursor(wrp->cursor.row, wrp->cursor.col);
	// int i;
	// for(i = 0; i < wrp->size; i++){
	// 	PutC(ios_tid, wrp->data[i]);
	// }
	// move_cursor(original->row, original->col);

	move_cursor(ios_tid, wrp->cursor.row, wrp->cursor.col);
	PRINTF(wrp->data);
	move_cursor(ios_tid, original->row, original->col);
}

void WriterServiceUART2(){
	int reply = 0;

	int r = RegisterAs(WRITERSERVICE_UART2_ID);
	assert(r == 0);

	tid_t ios_tid = WhoIs(IOSERVER_UART2_ID);

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
				PushCharToIO(ios_tid, &wrp, &cursor);
				Reply(req_tid, &reply, sizeof(reply));
				break;
			case WR_COMMAND:
				PushCommandToIO(ios_tid, &wrp, &cursor);
				Reply(req_tid, &reply, sizeof(reply));
				break;
			default:
				assert(0 && "Bad request");
				break;
		}
	}

	Exit();
}
