#include "unittestringbuffer.h"


void testIsOverflow(void){

    struct ring_buffer * buffer = initRingBuffer(5, INTERNAL_RING_BUFFER);

    for(int i = 0; i < 5; i++){
        printf("%f ", buffer->memory[i]);
    }
    printf("\n");

    addFloatToRingBuffer(buffer, 1);
    addFloatToRingBuffer(buffer, 2);
    addFloatToRingBuffer(buffer, 3);
    addFloatToRingBuffer(buffer, 4);
    addFloatToRingBuffer(buffer, 5);

    for(int i = 0; i < 5; i++){
        printf("%f ", buffer->memory[i]);
    }
    printf("\n");

    addFloatToRingBuffer(buffer, 6);
    addFloatToRingBuffer(buffer, 7);
    addFloatToRingBuffer(buffer, 8);
    
    for(int i = 0; i < 5; i++){
        printf("%f ", buffer->memory[i]);
    }
    printf("\n");

    freeRingBuffer(buffer);
}

