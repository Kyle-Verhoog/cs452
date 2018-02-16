

void rec_buffer_init(struct rec_buffer *rb) {
  int i;
  for (i = 0; i < REC_BUF_SIZE; i++)
    rb->buf[i] = -1;

  rb->start = 0;
  rb->num = 0;
}

void rec_buffer_add(struct rec_buffer *rb, int i) {
  rb->buf[rb->start] = i;
  rb->start = (rb->start + 1) % REC_BUF_SIZE;
  rb->num = rb->num + 1 > REC_BUF_SIZE ? rb->num : rb->num + 1;
}


int rec_buffer_get(struct rec_buffer *rb, int i) {
  int j = rb->start-1-i;
  int x = j < 0 ? REC_BUF_SIZE + j : (j % REC_BUF_SIZE);
  return rb->buf[x];
}

