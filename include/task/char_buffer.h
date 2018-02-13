#ifndef CHAR_BUFFER_H
#define CHAR_BUFFER_H
#include <lib/circular_buffer.h>

#define CHAR_BUFFER_SIZE 512

CIRCULAR_BUFFER_DEC(char_cb, char, CHAR_BUFFER_SIZE);

#endif
