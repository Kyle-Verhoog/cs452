#ifndef READER_SERVICE_H
#define READER_SERVICE_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>
#include <io/io.h>
#include <ascii.h>
#include <ioserver.h>

typedef enum ReadRequest{
	RR_CHAR = 0,
	RR_COMMAND = 1,
	RR_WRITE = 2
}ReadRequest;

typedef struct RSProtocol{
	ReadRequest rr_req;
	char data;
}RSProtocol;

void ReaderServiceUART2();

#endif //TIMER_INTERFACE_H
