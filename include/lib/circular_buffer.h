#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#define CB_E_NONE 0
#define CB_E_FULL 1
#define CB_E_EMPTY 2

#include <types.h>

// s/\s*\\$/\=repeat(' ', 60-col('.')).'\'

#define CIRCULAR_BUFFER_DEC(NAME, T, SIZE)                 \
typedef struct NAME {                                      \
  T buf[SIZE];                                             \
  uint32_t start;                                          \
  uint32_t end;                                            \
  uint32_t size;                                           \
} NAME;                                                    \
void NAME##_init(NAME *cb);                                \
int NAME##_push(NAME *cb, T val);                          \
int NAME##_top(NAME *cb, T *ret);                          \
int NAME##_pop(NAME *cb, T *ret);                          \

#define CIRCULAR_BUFFER_DEF(NAME, T, SIZE)                 \
void NAME##_init(NAME *cb) {                               \
  cb->start = 0;                                           \
  cb->end   = 0;                                           \
  cb->size  = 0;                                           \
}                                                          \
                                                           \
int NAME##_push(NAME *cb, T val) {                         \
  if(cb->size >= SIZE)                                     \
    return CB_E_FULL;                                      \
  cb->buf[cb->end] = val;                                  \
  cb->end = (cb->end + 1) % SIZE;                          \
  cb->size++;                                              \
  return CB_E_NONE;                                        \
}                                                          \
                                                           \
int NAME##_top(NAME *cb, T *ret) {                         \
  if (cb->size <= 0) return CB_E_EMPTY;                    \
  *ret = cb->buf[cb->start];                               \
  return CB_E_NONE;                                        \
}                                                          \
                                                           \
int NAME##_pop(NAME *cb, T *ret) {                         \
  if (cb->size <= 0)                                       \
    return CB_E_EMPTY;                                     \
  *ret = cb->buf[cb->start];                               \
  cb->start = (cb->start + 1) % SIZE;                      \
  cb->size--;                                              \
  return CB_E_NONE;                                        \
}                                                          \

#endif /* CIRCULAR_BUFFER_H */
