#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#define CB_E_NONE  0
#define CB_E_FULL  1
#define CB_E_EMPTY 2
#define CB_E_DNE   3
#define CB_E_OOB   4

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
int NAME##_get(NAME *cb, int i, T *ret);                   \
int NAME##_pop_end(NAME *cb, T *ret);                      \

#define CIRCULAR_BUFFER_DEF(NAME, T, SIZE)                 \
void NAME##_init(NAME *cb) {                               \
  cb->start = 0;                                           \
  cb->end   = 0;                                           \
  cb->size  = 0;                                           \
}                                                          \
                                                           \
int NAME##_push(NAME *cb, T val) {                         \
  if (cb->size >= SIZE)                                    \
    return CB_E_FULL;                                      \
  cb->buf[cb->end] = val;                                  \
  cb->end = (cb->end + 1) % SIZE;                          \
  cb->size++;                                              \
  return CB_E_NONE;                                        \
}                                                          \
                                                           \
int NAME##_get(NAME *cb, int i, T *ret) {                  \
  if (i > SIZE-1 || i < 0)                                 \
    return CB_E_OOB;                                       \
  i = (i + cb->start) % SIZE;                              \
  if ((cb->start > cb->end &&                              \
        i <= (cb->end-1+SIZE)%SIZE) ||                     \
      (cb->start == cb->end && cb->size == SIZE) ||        \
      (i < cb->end)) {                                     \
    *ret = cb->buf[i];                                     \
    return CB_E_NONE;                                      \
  }                                                        \
  return CB_E_DNE;                                         \
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
                                                           \
int NAME##_pop_end(NAME *cb, T *ret) {                     \
  if (cb->size <= 0)                                       \
    return CB_E_EMPTY;                                     \
  cb->end = (cb->end - 1 + SIZE) % SIZE;                   \
  *ret = cb->buf[cb->end];                                 \
  cb->size--;                                              \
  return CB_E_NONE;                                        \
}                                                          \

#endif /* CIRCULAR_BUFFER_H */
