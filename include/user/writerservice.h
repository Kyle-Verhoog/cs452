#ifndef WRITER_SERVICE_H
#define WRITER_SERVICE_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>
#include <io.h>
#include <ascii.h>
#include <ioserver.h>
#include <i2a.h>

typedef struct Cursor{
	int row;
	int col;
}Cursor;

typedef enum WriteRequest{
	WR_CHAR = 0,
	WR_COMMAND = 1,
}WriteRequest;

typedef struct WRProtocol{
	WriteRequest wr_req;
	char *data;
	int size;
	Cursor cursor;
}WRProtocol;

void WriterServiceUART2();
void WriteCharUART2(tid_t writer, char c);
void WriteCommandUART2(tid_t writer, char *command, int size, Cursor *cursor);

#define MAX_DIGITS 10




#endif //WRITER_SERVICE_H
