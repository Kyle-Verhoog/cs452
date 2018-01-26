#ifndef MSG_H
#define MSG_H

#include <kernel.h>

void send_handler(TaskDescriptor *std);

void receive_handler(TaskDescriptor *rtd);

void reply_handler(TaskDescriptor *rtd);

#endif /* MSG_H */
