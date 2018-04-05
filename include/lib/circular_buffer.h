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
  int r, u, v, start, size;                                \
  start = cb->start;                                       \
  size = cb->size;                                         \
  if (cb->size <= 0)                                       \
    return CB_E_DNE;                                       \
  u = start + size >= SIZE ? SIZE - start : size;          \
  v = start + size >= SIZE ? cb->end : 0;                  \
  r = start + u;                                           \
  i = i + start;                                           \
  if (i < r) {                                             \
    *ret = cb->buf[i];                                     \
    return CB_E_NONE;                                      \
  }                                                        \
  i = i - r;                                               \
  if (i < v) {                                             \
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

#define EXT_CIRCULAR_BUFFER_DEC(NAME, T, SIZE)             \
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
int NAME##_rem(NAME *cb, T val);                           \

#define EXT_CIRCULAR_BUFFER_DEF(NAME, T, SIZE)             \
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
  int r, u, v, start, size;                                \
  start = cb->start;                                       \
  size = cb->size;                                         \
  if (cb->size <= 0)                                       \
    return CB_E_DNE;                                       \
  u = start + size >= SIZE ? SIZE - start : size;          \
  v = start + size >= SIZE ? cb->end : 0;                  \
  r = start + u;                                           \
  i = i + start;                                           \
  if (i < r) {                                             \
    *ret = cb->buf[i];                                     \
    return CB_E_NONE;                                      \
  }                                                        \
  i = i - r;                                               \
  if (i < v) {                                             \
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
                                                           \
int NAME##_rem(NAME *cb, T val) {                          \
  int i, u, v, j, start, size;                             \
  start = cb->start;                                       \
  size = cb->size;                                         \
  if (cb->size <= 0)                                       \
    return CB_E_EMPTY;                                     \
  u = start + (start + size >= SIZE ? SIZE - start : size);\
  v = start + size >= SIZE ? cb->end : 0;                  \
  i = start;                                               \
  j = 0;                                                   \
  for (; i < u; ++i) {                                     \
    if (cb->buf[i] == val)                                 \
      goto SHIFT;                                          \
  }                                                        \
  for (; j < v; ++j) {                                     \
    if (cb->buf[j] == val)                                 \
      goto SHIFT;                                          \
  }                                                        \
  return CB_E_DNE;                                         \
                                                           \
SHIFT:                                                     \
  for (; i < u-1; ++i)                                     \
    cb->buf[i] = cb->buf[i+1];                             \
  if (i == u-1 && j < v)                                   \
    cb->buf[u-1] = cb->buf[0];                             \
  for (; j < v-1; ++j)                                     \
    cb->buf[j] = cb->buf[j+1];                             \
  cb->end = (cb->end - 1 + SIZE) % SIZE;                   \
  cb->size--;                                              \
  return CB_E_NONE;                                        \
}                                                          \


#endif /* CIRCULAR_BUFFER_H */
