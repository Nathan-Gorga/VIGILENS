#include "../main.h"
#include "../RINGBUFFER/ringbuffer.h"

int main(void){
    printf("Hello world\n");



    struct ring_buffer * buffer = initRingBuffer(5, EVENT_RING_BUFFER);

    buffer->write = 0;

    printf("%d\n",writeIndexAfterDecrement(buffer));
    buffer->write = 1;
    printf("%d\n",writeIndexAfterDecrement(buffer));

    buffer->write = buffer->size - 1;
    printf("%d\n",writeIndexAfterDecrement(buffer));
    

    return 0;
}