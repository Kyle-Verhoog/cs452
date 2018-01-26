#include <k2_task.h>

void InitTask(){
	//Create the nameserver
	Create(31, &NameServerTask);

	//Kick off the rpc Server
	Create(30, &RPCServer);

	//Create our clients
	Create(5, &RPCClient);
	Create(5, &RPCClient);

	//Send to server to exit
	int reply;
	RPCreq req;
	req.type = S_Close;
	send(2, &req, sizeof(RPCreq), &reply, sizeof(int));	

	Exit();
}

void ResetMatch(RPCmatch* match){
	match->ready = false;
	match->p1Play = false;
	match->p2Play = false;
}

int RPCGetWinner(RPCmove p1, RPCmove p2){
	if(p1 == Rock && p2 == Paper){
		return 2;
	}
	else if(p1 == Rock && p2 == Scissor){
		return 1;
	}
	else if(p1 == Paper && p2 == Rock){
		return 1;
	}
	else if(p1 == Paper && p2 == Scissor){
		return 2;
	}
	else if(p1 == Scissor && p2 == Rock){
		return 2;
	}
	else if(p1 == Scissor && p2 == Paper){
		return 1;
	}
	
	//Tie
	return -1;
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
		reply(match->tidOne, &match->tidTwo, sizeof(int));
		reply(match->tidTwo, &match->tidOne, sizeof(int));
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
		reply(match->tidOne, &res, sizeof(RPCresult));	
		reply(match->tidTwo, &res, sizeof(RPCresult));
		ResetMatch(match);
	}
}

void RPCQuit(RPCmatch *match, int playerTid){
	RPCresult res;
	res.gameResult = Quit;
	res.playerOfFocus = playerTid;

	//notify players and early out
	reply(match->tidOne, &res, sizeof(RPCresult));
	reply(match->tidTwo, &res, sizeof(RPCresult));

	ResetMatch(match);
}

void RPCClient(){
	RPCreq req;
	RPCresult result;

	int replyOne;
	req.type = S_Signup;
	send(2, &req, sizeof(RPCreq), &replyOne, sizeof(int));

	req.type = S_Play;
	req.move = MyTid() % 3;
	send(2, &req, sizeof(RPCreq), &result, sizeof(RPCresult));

	bwprintf(COM2, "Winner %x\n\r", result.playerOfFocus);

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
		receive(&requestor, &request, sizeof(RPCreq));

		switch(request.type){
			case S_Signup:
				RPCSignup(&cb, &match, requestor);
				break;
			case S_Play:
				RPCPlay(&match, requestor, request.move);
				break;
			case S_Quit:
				RPCQuit(&match, requestor);
				break;
			case S_Close:
				finish = true;
				reply(requestor, &finish, sizeof(int));
				break;
			default:
				assert(0);
				break;
		}
	}

	Exit();
}