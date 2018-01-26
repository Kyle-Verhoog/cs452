#ifndef K2_TASK_H
#define K2_TASK_H
#include <bwio.h>
#include <kernel/syscalls.h>
#include <user/nameserver.h>
#include <lib/circularbuffer.h>

typedef enum RPCservice{
	S_Signup = 0,
	S_Play = 1,
	S_Quit = 2,
	S_Close = 3
}RPCservice;

typedef enum RPCmove{
	M_Rock = 0,
	M_Paper = 1,
	M_Scissor = 2,
	M_Quit = 3
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
void RPCClient2();
void RPCServer();

#endif /* K2_TASK_H */
