/*
 * bwio.h
 */

#ifndef BWIO_H
#define BWIO_H

typedef char *va_list;

#define __va_argsiz(t)  \
  (((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, pN) ((ap) = ((va_list) __builtin_next_arg(pN)))

#define va_end(ap)  ((void)0)

#define va_arg(ap, t) \
  (((ap) = (ap) + __va_argsiz(t)), *((t*) (void*) ((ap) - __va_argsiz(t))))

#define COM1  0
#define COM2  1

#define ON  1
#define OFF 0

/* ep93xx-user-guide.pdf page 652 */
#define RTC_BASE = 0x80920000
#define RTC_LOAD = 0x8092000c
#define RTC_CTRL = 0x80920010


/* ep93xx-user-guide.pdf page 636 */
#define TIMER3_DATA 0x80810084
#define TIMER3_LOAD 0x80810080
#define TIMER3_CTRL 0x80810088

#define TIMER3_MASK 0x80
#define TIMER3_INIT 0xffffffff

int bwsetfifo( int channel, int state );

int bwsetspeed( int channel, int speed );

int bwputc( int channel, char c );

int bwgetc( int channel );

int bwputx( int channel, char c );

int bwputstr( int channel, const char *str );

int bwputr( int channel, unsigned int reg );

void bwputw( int channel, int n, char fc, const char *bf );

void bwformat ( int channel, const char *fmt, va_list va );

void bwprintf( int channel, const char *format, ... );


#endif /* BWIO_H */
