#ifndef SWITCH_H
#define SWITCH_H

typedef enum SwitchState{
	SW_STRAIGHT = 33,
	SW_CURVE = 34
} SwitchState;

typedef struct Switch{
  int state;
  track_node *branch;
  track_node *merge;
}Switch;

typedef enum SW_Request{
	SW_HALT = -1,
	SW_FLIP = 0,
	SW_GET_ALL = 1,
	//User Reqs
	SW_NOTIFY_READY = 3,
	SW_NOTIFY = 4,
	SW_SUBSCRIBE = 5,
}SW_Request;

typedef struct SWProtocol{
	SW_Request swr;
	char sw;
	SwitchState dir;
}SWProtocol;

typedef struct SWSubscribe{
	SW_Request swr;
	SWProtocol swp;
}

#endif
