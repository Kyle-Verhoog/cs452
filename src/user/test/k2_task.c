#include <k2_task.h>
#include <syscalls.h>
#include <ts7200.h>

void InitTask(){
	bwsetfifo( COM2, OFF );
	bwsetfifo( COM1, OFF );

	//Create the nameserver
	//Create(31, &NameServerTask);

	//Kick off the rpc Server
	Create(30, &RPCServer);

	//Create our clients
	int i = 0;
	for(i = 0; i < 50; i++){
		if(i%3 == 0){
			Create(5, &RPCClient2);
		}else{
			Create(5, &RPCClient);		
		}
	}


	//Send to server to exit
	int reply;
	RPCreq req;
	req.type = S_Close;
	Send(1, &req, sizeof(RPCreq), &reply, sizeof(int));	

	Exit();
}

void ResetMatch(RPCmatch* match){
	match->ready = false;
	match->p1Play = false;
	match->p2Play = false;
}

int RPCGetWinner(RPCmove p1, RPCmove p2){	
	if(p1 == M_Quit){
		return -1;
	}
	else if(p2 == M_Quit){
		return -2;
	}
	else if(p1 == M_Rock && p2 == M_Paper){
		return 2;
	}
	else if(p1 == M_Rock && p2 == M_Scissor){
		return 1;
	}
	else if(p1 == M_Paper && p2 == M_Rock){
		return 1;
	}
	else if(p1 == M_Paper && p2 == M_Scissor){
		return 2;
	}
	else if(p1 == M_Scissor && p2 == M_Rock){
		return 2;
	}
	else if(p1 == M_Scissor && p2 == M_Paper){
		return 1;
	}
	
	//Tie
	return 0;
}

void RPCSignup(CircularBuffer *cb, RPCmatch* match, int requestor){
	push_circularBuffer(cb, requestor);

	if(cb->size >= 2 && match->ready != true){
		match->tidOne = top_circularBuffer(cb);
		pop_circularBuffer(cb);
		match->tidTwo = top_circularBuffer(cb);
		pop_circularBuffer(cb);
		match->ready = true;

		//Reply to our waiting players
		Reply(match->tidOne, &match->tidTwo, sizeof(int));
		Reply(match->tidTwo, &match->tidOne, sizeof(int));
	}
}

void RPCPlay(RPCmatch* match, int playerTid, RPCmove move){
	RPCresult res;

	//Fill in the move
	if(match->tidOne == playerTid){
		match->p1Move = move;
		match->p1Play = true;
	}else if(match->tidTwo == playerTid){
		match->p2Move = move;
		match->p2Play = true;
	}else{
		assert(0);
	}

	//Check if we can play the round
	if(match->p1Play && match->p2Play){
		switch(RPCGetWinner(match->p1Move, match->p2Move)){
			case -2:
				res.gameResult = Quit;
				res.playerOfFocus = match->tidTwo;
				break;
			case -1:
				res.gameResult = Quit;
				res.playerOfFocus = match->tidOne;
				break;
			case 1:
				res.gameResult = Winner;
				res.playerOfFocus = match->tidOne;
				break;
			case 2:
				res.gameResult = Winner;
				res.playerOfFocus = match->tidTwo;	
				break;
			default:
				res.gameResult = Tie;
				break;
		}
		Reply(match->tidOne, &res, sizeof(RPCresult));	
		Reply(match->tidTwo, &res, sizeof(RPCresult));

		bwprintf(COM2, "Press Any Key to Continue: ");
		char c = bwgetc(COM2);
		ResetMatch(match);
	}
}

void RPCQuit(RPCmatch *match, int playerTid){
	RPCPlay(match, playerTid, M_Quit);
}

void RPCClient(){
	RPCreq req;
	RPCresult result;
	int tid = MyTid();

	int replyOne;
	req.type = S_Signup;
	Send(1, &req, sizeof(RPCreq), &replyOne, sizeof(int));

	req.type = S_Play;
	req.move = MyTid() % 3;
	Send(1, &req, sizeof(RPCreq), &result, sizeof(RPCresult));

	if(result.gameResult == Quit && result.playerOfFocus != tid){
		bwprintf(COM2, "Task - %x quit on me!\n\r", result.playerOfFocus);
	}else if(result.playerOfFocus == tid){
		bwprintf(COM2, "I (Task - %x) Won!!\n\r", result.playerOfFocus);
	}

	Exit();
}

void RPCClient2(){
	RPCreq req;
	RPCresult result;
	int tid = MyTid();

	int replyOne;
	req.type = S_Signup;
	Send(1, &req, sizeof(RPCreq), &replyOne, sizeof(int));

	req.type = S_Quit;
	Send(1, &req, sizeof(RPCreq), &result, sizeof(RPCresult));

	Exit();
}

void RPCServer(){
	int finish = false;
	CircularBuffer cb;
	RPCmatch match;	//currently on-going match
	init_circularBuffer(&cb);
	ResetMatch(&match);

	while(true){
		int requestor;
		RPCreq request;

		//wait for input
		Receive(&requestor, &request, sizeof(RPCreq));

		switch(request.type){
			case S_Signup:
				RPCSignup(&cb, &match, requestor);
				break;
			case S_Play:
				RPCPlay(&match, requestor, request.move);
				break;
			case S_Quit:
				RPCQuit(&match, requestor);
				//RPCPlay(&match, requestor, M_Quit);
				break;
			case S_Close:
				finish = true;
				Reply(requestor, &finish, sizeof(int));
				Exit();
			default:
				assert(0);
				break;
		}
	}	
}