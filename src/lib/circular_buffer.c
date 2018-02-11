#include <lib/circular_buffer.h>

/*
void cbuf_init(circular_buffer *cb, void *buffer, size_t el_size, uint32_t cap) {
  cb->buf = buffer;
  cb->start = 0;
  cb->end   = 0;
  cb->size  = 0;
  cb->cap   = cap;
  cb->el_size = el_size;
}

int cbuf_push(circular_buffer *cb, void *val) {
  if(cb->size < cb->cap) {
    // cb->buf[cb->end] = val;
    cb->end = (cb->end + 1) % cb->cap;
    cb->size++;
    return CB_E_NONE;
  }

  return CB_E_FULL;
}

// unsigned int cbuf_top(circular_buffer *cb) {
//   return cb->buf[cb->start];
// }

int cbuf_pop(circular_buffer *cb) {
  if(cb->size > 0) {
    cb->start = (cb->start + 1) % cb->cap;
    cb->size--;
    return CB_E_NONE;
  }

  return CB_E_EMPTY;
}
*/
