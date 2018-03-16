#ifndef SWITCH_H
#define SWITCH_H

typedef enum SwitchState{
	DIR_STRAIGHT = 0,
	DIR_CURVED = 1,
}SwitchState;

typedef struct Switch{
	int state;
	track_node *branch;
	track_node *merge;
}Switch;

#endif
