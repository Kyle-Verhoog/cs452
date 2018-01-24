#include <assert.h>
#include <test/circularbuffer.h>

void cb_sanity(){
	CircularBuffer cb;
	init_circularBuffer(&cb);

	int i;
	for(i = 0; i < CIRCULAR_BUFFER_SIZE; i++){
		push_circularBuffer(&cb, i);
	}

	while(cb.buffer_end != cb.buffer_start){
		assert(CIRCULAR_BUFFER_SIZE - i == top_circularBuffer(&cb));
		pop_circularBuffer(&cb);
	}

}

void circular_buffer_tests() {
  cb_sanity();
	printf("✓ CircularBuffer\n");
}
