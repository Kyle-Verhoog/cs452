#ifndef HANDLER_NAMESERVER_H
#define HANDLER_NAMESERVER_H

#include <kernel/kernel.h>

tid_t nameserver_tid;

void ns_register(TaskDescriptor *td);

void ns_get(TaskDescriptor *td);

#endif
