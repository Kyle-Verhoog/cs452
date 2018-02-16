#ifndef REC_BUFFER_H
#define REC_BUFFER_H

#define REC_BUF_SIZE 20

typedef struct rec_buffer{
	int buf[REC_BUF_SIZE];
	int start;
	int num;
} rec_buffer;

void rec_buffer_init(struct rec_buffer *rb);

void rec_buffer_add(struct rec_buffer *rb, int i);

int rec_buffer_get(struct rec_buffer *rb, int i);
#endif
