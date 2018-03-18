#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <system.h>
#include <user/syscalls.h>
#include <user/nameserver.h>
#include <user/terminal_manager.h>
#include <user/train/events.h>
#include <user/train/waiting_room.h>
#include <user/train/representer.h>


#define VIRTUAL_EVENT_LIST_SIZE 10

CIRCULAR_BUFFER_DEC(VEList, VirtualEvent, VIRTUAL_EVENT_LIST_SIZE);

void Interpreter();

#endif
