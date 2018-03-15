/* source: http://beej.us/guide/bgnet/examples/pack2.c
 */

#include <lib/pack.h>

int isdigit(int i) {
  return '0' <= i &&  i <= '9';
}

// various bits for floating point types--varies for different architectures
// typedef float float32_t;
// typedef double float64_t;

// macros for packing floats and doubles:
// #define pack754_32(f) (pack754((f), 32, 8))
// #define pack754_64(f) (pack754((f), 64, 11))
// #define unpack754_32(i) (unpack754((i), 32, 8))
// #define unpack754_64(i) (unpack754((i), 64, 11))

/**
 * pack754() -- pack a floating point number into IEEE-754 format
 */
// uint64_t pack754(long double f, unsigned bits, unsigned expbits)
// {
//   long double fnorm;
//   int shift;
//   long long sign, exp, significand;
//   unsigned significandbits = bits - expbits - 1; // -1 for sign bit
// 
//   if (f == 0.0) return 0; // get this special case out of the way
// 
//   // check sign and begin normalization
//   if (f < 0) { sign = 1; fnorm = -f; }
//   else { sign = 0; fnorm = f; }
// 
//   // get the normalized form of f and track the exponent
//   shift = 0;
//   while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
//   while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
//   fnorm = fnorm - 1.0;
// 
//   // calculate the binary form (non-float) of the significand data
//   significand = fnorm * ((1LL<<significandbits) + 0.5f);
// 
//   // get the biased exponent
//   exp = shift + ((1<<(expbits-1)) - 1); // shift + bias
// 
//   // return the final answer
//   return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
// }

/*
** unpack754() -- unpack a floating point number from IEEE-754 format
*/ 
// long double unpack754(uint64_t i, unsigned bits, unsigned expbits)
// {
//   long double result;
//   long long shift;
//   unsigned bias;
//   unsigned significandbits = bits - expbits - 1; // -1 for sign bit
// 
//   if (i == 0) return 0.0;
// 
//   // pull the significand
//   result = (i&((1LL<<significandbits)-1)); // mask
//   result /= (1LL<<significandbits); // convert back to float
//   result += 1.0f; // add the one back on
// 
//   // deal with the exponent
//   bias = (1<<(expbits-1)) - 1;
//   shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
//   while(shift > 0) { result *= 2.0; shift--; }
//   while(shift < 0) { result /= 2.0; shift++; }
// 
//   // sign it
//   result *= (i>>(bits-1))&1? -1.0: 1.0;
// 
//   return result;
// }

/*
** packi16() -- store a 16-bit int into a char buffer (like htons())
*/ 
void packi16(uint8_t *buf, int16_t i)
{
  uint16_t i2 = i;

  *buf++ = i2>>8; *buf++ = i2;
}

/*
** packi32() -- store a 32-bit int into a char buffer (like htonl())
*/ 
void packi32(uint8_t *buf, int32_t i)
{
  uint32_t i2 = i;

  *buf++ = i2>>24; *buf++ = i2>>16;
  *buf++ = i2>>8;  *buf++ = i2;
}

/*
** packi64() -- store a 64-bit int into a char buffer (like htonl())
*/ 
// void packi64(uint8_t *buf, int64_t i)
// {
//   uint64_t i2 = i;
// 
//   *buf++ = i2>>56; *buf++ = i2>>48;
//   *buf++ = i2>>40; *buf++ = i2>>32;
//   *buf++ = i2>>24; *buf++ = i2>>16;
//   *buf++ = i2>>8;  *buf++ = i2;
// }

/*
** unpacki16() -- unpack a 16-bit int from a char buffer (like ntohs())
*/ 
int16_t unpacki16(uint8_t *buf)
{
  uint16_t i2 = ((uint16_t)buf[0]<<8) | buf[1];
  int16_t i;

  // change unsigned numbers to signed
  if (i2 <= 0x7fffu) { i = i2; }
  //else { i = -(int16_t)((uint16_t)0xffff - i2 + (uint16_t)1u); }
  else { i = -1 - (uint16_t)(0xffffu - i2); }

  return i;
}

/*
** unpacki32() -- unpack a 32-bit int from a char buffer (like ntohl())
*/ 
int32_t unpacki32(uint8_t *buf)
{
  uint32_t i2 = ((uint32_t)buf[0]<<24) | ((uint32_t)buf[1]<<16) |
    ((uint32_t)buf[2]<<8)  | buf[3];
  int32_t i;

  // change unsigned numbers to signed
  if (i2 <= 0x7fffffffu) { i = i2; }
  else { i = -1 - (int32_t)(0xffffffffu - i2); }

  return i;
}

/*
** unpacki64() -- unpack a 64-bit int from a char buffer (like ntohl())
*/ 
// int64_t unpacki64(uint8_t *buf)
// {
//   uint64_t i2 = ((uint64_t)buf[0]<<56) | ((uint64_t)buf[1]<<48) |
//     ((uint64_t)buf[2]<<40) | ((uint64_t)buf[3]<<32) |
//     ((uint64_t)buf[4]<<24) | ((uint64_t)buf[5]<<16) |
//     ((uint64_t)buf[6]<<8)  | buf[7];
//   int64_t i;
// 
//   // change unsigned numbers to signed
//   if (i2 <= 0x7fffffffffffffffu) { i = i2; }
//   else { i = -1 -(int64_t)(0xffffffffffffffffu - i2); }
// 
//   return i;
// }

/*
** pack() -- store data dictated by the format string in the buffer
**
**  c - 8-bit signed int     h - 16-bit signed int
**  l - 32-bit signed int    f - 32-bit float
**  L - 64-bit signed int    F - 64-bit float
**  s - string (16-bit length is automatically prepended)
*/ 
int32_t pack(uint8_t *buf, char *format, ...)
{
  va_list ap;
  // int16_t h;
  int32_t l;
  // int64_t L;
  int8_t c;
  // float32_t f;
  // float64_t F;
  char *s;
  int32_t size = 0, len;

  va_start(ap, format);

  for(; *format != '\0'; format++) {
    switch(*format) {
    // case 'h': // 16-bit
    //   size += 2;
    //   h = (int16_t)va_arg(ap, int); // promoted
    //   packi16(buf, h);
    //   buf += 2;
    //   break;

    case 'd': // 32-bit
      size += 4;
      l = va_arg(ap, int32_t);
      packi32(buf, l);
      buf += 4;
      break;

    // case 'L': // 64-bit
    //   size += 8;
    //   L = va_arg(ap, int64_t);
    //   packi64(buf, L);
    //   buf += 8;
    //   break;

    case 'c': // 8-bit
      size += 1;
      c = (int8_t)va_arg(ap, int); // promoted
      *buf++ = (c>>0)&0xff;
      break;

    // case 'f': // float
    //   size += 4;
    //   f = (float32_t)va_arg(ap, double); // promoted
    //   l = pack754_32(f); // convert to IEEE 754
    //   packi32(buf, l);
    //   buf += 4;
    //   break;

    // case 'F': // float-64
    //   size += 8;
    //   F = (float64_t)va_arg(ap, float64_t);
    //   L = pack754_64(F); // convert to IEEE 754
    //   packi64(buf, L);
    //   buf += 8;
    //   break;

    case 's': // string
      s = va_arg(ap, char*);
      len = strlen(s);
      size += len + 2;
      packi16(buf, len);
      buf += 2;
      memcopy(buf, s, len);
      buf += len;
      break;
    default:
      break;
    }
  }

  va_end(ap);

  return size;
}

/*
** unpack() -- unpack data dictated by the format string into the buffer
*/
void unpack(uint8_t *buf, char *format, ...)
{
  va_list ap;
  // int16_t *h;
  int32_t *l;
  // int64_t *L;
  // int32_t pf;
  // int64_t pF;
  int8_t *c;
  // float32_t *f;
  // float64_t *F;
  char *s;
  int32_t len, count, maxstrlen=0;

  va_start(ap, format);

  for(; *format != '\0'; format++) {
    switch(*format) {
    // case 'h': // 16-bit
    //   h = va_arg(ap, int16_t*);
    //   *h = unpacki16(buf);
    //   buf += 2;
    //   break;

    case 'd': // 32-bit
      l = va_arg(ap, int32_t*);
      *l = unpacki32(buf);
      buf += 4;
      break;

    // case 'L': // 64-bit
    //   L = va_arg(ap, int64_t*);
    //   *L = unpacki64(buf);
    //   buf += 8;
    //   break;

    case 'c': // 8-bit
      c = va_arg(ap, int8_t*);
      if (*buf <= 0x7f) { *c = *buf;}
      else { *c = -1 - (uint8_t)(0xffu - *buf); }
      buf++;
      break;

    // case 'f': // float
    //   f = va_arg(ap, float32_t*);
    //   pf = unpacki32(buf);
    //   buf += 4;
    //   *f = unpack754_32(pf);
    //   break;

    // case 'F': // float-64
    //   F = va_arg(ap, float64_t*);
    //   pF = unpacki64(buf);
    //   buf += 8;
    //   *F = unpack754_64(pF);
    //   break;

    case 's': // string
      s = va_arg(ap, char*);
      len = unpacki16(buf);
      buf += 2;
      if (maxstrlen > 0 && len > maxstrlen) count = maxstrlen - 1;
      else count = len;
      memcopy(s, buf, count);
      s[count] = '\0';
      buf += len;
      break;

    default:
      if (isdigit(*format)) { // track max str len
        maxstrlen = maxstrlen * 10 + (*format-'0');
      }
    }

    if (!isdigit(*format)) maxstrlen = 0;
  }

  va_end(ap);
}
