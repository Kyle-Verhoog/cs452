#include <writeback_interface.h>

void WritebackInterface(){
	int reply;
	int in_tid = WhoIs(READERSERVICE_UART2_ID);

	tid_t reader;
	char c;

	//Keep track of Cursor - all other tasks should move cursor back here after use
	int cur_row = 1;
	int cur_col = 1;

	RSProtocol rsp;
	rsp.rr_req = RR_CHAR;

	//Move cursor to starting location

	//Register to ReaderService
	Send(in_tid, &rsp, sizeof(rsp), &reply, sizeof(reply));
	assert(reply == 0);

	while(true){
		Receive(&reader, &c, sizeof(c));
		Reply(reader, &reply, sizeof(reply));

		switch(c){
			case BACKSPACE:
				//Move cursor back twice
				PRINTF("%c", c);
				//Move cursor back once
				break;
			case CARRIAGE_RETURN:
				//Clear the line
				cur_col = 1;
				PRINTF("\n\r", c);
				break;
			default:
				PRINTF("%c", c);
				break;
		}
	}

	Exit();
}
