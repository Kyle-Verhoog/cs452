#include<train_msg.h>

void TrainMsg(){
	int reply;
	int in_tid = WhoIs(READERSERVICE_UART2_ID);

	tid_t reader;
	char command[COMMAND_SIZE];
	int size;

	RSProtocol rsp;
	rsp.rr_req = RR_COMMAND;

	//Register to ReaderService
	Send(in_tid, &rsp, sizeof(rsp), &reply, sizeof(reply));
	assert(reply == 0);

	while(true){
		//Read size of string, then the string itself
		Receive(&reader, &size, sizeof(size));
		Reply(reader, &reply, sizeof(reply));
		Receive(&reader, &command, size);
		Reply(reader, &reply, sizeof(reply));

		//Append c string terminator character
		command[size] = '\0';

		PRINTF("%s", command);
	}

	Exit();
}

