#include "unittestringbuffer.h"


void testIsOverflow(void){

    struct ring_buffer * buffer = initRingBuffer(5, EVENT_RING_BUFFER);

  
    float data[] = {1,2,3,4,5};

    addBufferToRingBuffer(buffer, data, 5);

    //
    int size = 5;
    float get1[size];
    extractBufferFromRingBuffer(buffer, get1,size, 2,1);

    for(int i=0; i<size; i++){
        printf("%f ", get1[i]);
    }

    printf("\n\n");
    

    freeRingBuffer(buffer);
}

