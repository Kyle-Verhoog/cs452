#ifndef HANDLER_CREATE_H
#define HANDLER_CREATE_H

#include <kernel/kernel.h>
#include <task/task.h>

void create(TaskDescriptor *td);

void create_w_args(TaskDescriptor *td);

#endif /* HANDLER_CREATE_H */
