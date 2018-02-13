/*
 * bwio.h
 */

#ifndef BWIO_H
#define BWIO_H

#include <system.h>

#define COM1  0
#define COM2  1
#define COM3  2

#define ON  1
#define OFF 0

int bwsetfifo( int channel, int state );

int bwsetspeed( int channel, int speed );

int bwputc( int channel, char c );

int bwgetc( int channel );

int bwputx( int channel, char c );

int bwputstr( int channel, const volatile char *str );

int bwputr( int channel, unsigned int reg );

void bwputw( int channel, int n, char fc, const char *bf );

void bwformat ( int channel, const volatile char *fmt, va_list va );

void bwprintf( int channel, const volatile char *format, ... );


#endif /* BWIO_H */
