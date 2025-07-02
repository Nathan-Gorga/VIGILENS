#include "unittestringbuffer.h"


void testIsOverflow(void){

    struct ring_buffer * buffer = initRingBuffer(5, EVENT_RING_BUFFER);

    for(int i = 0; i < 5; i++){
        printf("%f ", buffer->memory[i]);
    }printf("\n");

    float data[] = {1,2,3,4,5};

    addBufferToRingBuffer(buffer, data, 5);

    for(int i = 0; i < 5; i++){
        printf("%f ", buffer->memory[i]);
    }printf("\n");


    float data2[] = {6,7,8};

    addBufferToRingBuffer(buffer, data2, 3);

    for(int i = 0; i < 5; i++){
        printf("%f ", buffer->memory[i]);
    }printf("\n");

    float data3[] = {9,10,11,12,13,14,15,16};


    addBufferToRingBuffer(buffer, data3, 8);

    for(int i = 0; i < 5; i++){
        printf("%f ", buffer->memory[i]);
    }printf("\n");

    freeRingBuffer(buffer);
}

