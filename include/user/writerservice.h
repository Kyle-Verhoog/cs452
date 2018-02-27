#ifndef WRITER_SERVICE_H
#define WRITER_SERVICE_H

#include <ts7200.h>
#include <defines.h>
#include <user/syscalls.h>
#include <io.h>
#include <ascii.h>
#include <ioserver.h>
#include <i2a.h>

#define WRITEBACK_ROW 30
#define WRITEBACK_COL 1

#define SET_CURSOR(c, x, y) c.row = x; \
							c.col = y;

#define SHIFT_CURSOR(c, x, y) c.row += x; \
							c.col += y;							

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

void WriteCommandUART1(tid_t tx_tid, char *command, int size);
void WriteCharUART2(tid_t writer, char c);
void WriteCommandUART2(tid_t writer, char *command, int size, Cursor *cursor);
void WriteStringUART2(tid_t writer, char *command, Cursor *cursor);

#define MAX_DIGITS 10




#endif //WRITER_SERVICE_H
