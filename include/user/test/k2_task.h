#ifndef K2_TASK_H
#define K2_TASK_H
#include <bwio.h>
#include <kernel/syscalls.h>
#include <nameserver/nameserver.h>
#include <lib/circularbuffer.h>

typedef enum RPCservice{
	S_Signup = 0,
	S_Play = 1,
	S_Quit = 2,
	S_Close = 3
}RPCservice;

typedef enum RPCmove{
	Rock = 0,
	Paper = 1,
	Scissor = 2
}RPCmove;

typedef enum RPCfinish{
	Quit = 0,
	Winner = 1,
	Tie = 2
}RPCfinish;

typedef struct RPCreq{
	RPCservice type;
	RPCmove move;
}RPCreq;

typedef struct RPCmatch{
	int tidOne;
	int tidTwo;
	int ready;

	//Game State
	RPCmove p1Move;
	int p1Play;
	RPCmove p2Move;
	int p2Play;
}RPCmatch;

typedef struct RPCresult{
	RPCfinish gameResult;
	int playerOfFocus;
}RPCresult;

//All Tasks
void InitTask();
void RPCClient();
void RPCServer();

#endif /* K2_TASK_H */
