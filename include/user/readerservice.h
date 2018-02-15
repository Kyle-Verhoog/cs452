#ifndef READER_SERVICE_H
#define READER_SERVICE_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>
#include <io/io.h>
#include <ascii.h>
#include <ioserver.h>
#include <writerservice.h>

typedef enum ReadRequest{
	RR_CHAR = 0,
	RR_COMMAND = 1,
	RR_WRITE = 2
}ReadRequest;

typedef struct RSProtocol{
	ReadRequest rr_req;
	char data;
}RSProtocol;

typedef struct RSResponse{
	char *data;
	int size;
}RSResponse;

void ReaderServiceUART2();
void ReaderServiceUART2Register(RSProtocol *rsp);
void ReadChar(char *c);
void ReadCommand(char *command, int *size);

#endif //READER_SERVICE_H
