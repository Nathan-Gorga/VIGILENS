#include "unittestringbuffer.h"


void testIsOverflow(void){

    struct ring_buffer * buffer = initRingBuffer(5, INTERNAL_RING_BUFFER);

    for(int i = 0; i < 10; i++){
        printf("write index = %d\n", buffer->write);
        writeIndexIncrement(buffer);
    }


    freeRingBuffer(buffer);
}

