#ifndef BUFFER_PACK_H
#define BUFFER_PACK_H

#include <lib/va_arg.h>

int buf_pack_c(char *buf, char c);

int buf_pack_ui32(char *buf, unsigned int i, unsigned int base);

int buf_pack_i32(char *buf, int i);

/**
 * Adds a string of characters to the buffer.
 * returns 0 if successful
 *         1 if the buffer size has been exceeded
 */
int buf_pack_str(char *buf, const char *str);

int buf_pack_fmt(char *buf, char *fmt, va_list va);

int buf_pack_f(char *buf, char *fmt, ...);

#endif  /* BUFFER_PACK_H */
