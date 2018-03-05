#ifndef IOSERVER_BUFFER_H
#define IOSERVER_BUFFER_H

#include <lib/circular_buffer.h>

#define IO_SERVER_BUFFER_SIZE 16192

typedef struct iobuf_item {
  char c;
  tid_t btid;
} iobuf_item;

CIRCULAR_BUFFER_DEC(io_cb, iobuf_item, IO_SERVER_BUFFER_SIZE);

#endif
