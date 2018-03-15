#ifndef PACK_H
#define PACK_H

#include <stdlib.h>

// various bits for floating point types--varies for different architectures
typedef float float32_t;
typedef double float64_t;

// macros for packing floats and doubles:
// #define pack754_16(f) (pack754((f), 16, 5))
// #define pack754_32(f) (pack754((f), 32, 8))
// #define pack754_64(f) (pack754((f), 64, 11))
// #define unpack754_16(i) (unpack754((i), 16, 5))
// #define unpack754_32(i) (unpack754((i), 32, 8))
// #define unpack754_64(i) (unpack754((i), 64, 11))


int32_t unpacki32(uint8_t *buf);

/*
** pack() -- store data dictated by the format string in the buffer
**
**   bits |signed   unsigned   float   string
**   -----+----------------------------------
**      8 |   c        C         
**     16 |   h        H         
**     32 |   l        L         f
**     64 |   q        Q         F
**      - |                               s
**
**  (16-bit unsigned length is automatically prepended to strings)
*/ 
int32_t pack(unsigned char *buf, char *format, ...);

/*
** unpack() -- unpack data dictated by the format string into the buffer
**
**   bits |signed   unsigned   float   string
**   -----+----------------------------------
**      8 |   c        C
**     16 |   h        H
**     32 |   l        L         f
**     64 |   q        Q         F
**      - |                               s
**
**  (string is extracted based on its stored length, but 's' can be
**  prepended with a max length)
*/
void unpack(unsigned char *buf, char *format, ...);
#endif
