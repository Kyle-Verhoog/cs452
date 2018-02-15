#ifndef UIO_H
#define UIO_H

#include <ts7200.h>
#include <types.h>
#include <lib/va_arg.h>
#include <user/syscalls.h>
#include <io.h>
#include <user/ioserver.h>
#include <system.h>

int putc(tid_t ios_tid, char c);

int getc(tid_t ios_tid);

int putx(tid_t ios_tid, char c);

int putstr(tid_t ios_tid, const volatile char *str);

int putr(tid_t ios_tid, unsigned int reg);

void putw(tid_t ios_tid, int n, char fc, const char *bf);

void format(tid_t ios_tid, const volatile char *fmt, va_list va);

void print(tid_t ios_tid, const volatile char *format, ...);

extern tid_t UART2_TX;

#endif /* UIO_H */
