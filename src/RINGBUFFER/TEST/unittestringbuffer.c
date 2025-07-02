#include "unittestringbuffer.h"


void testIsOverflow(void){

    struct ring_buffer * buffer = initRingBuffer(10, INTERNAL_RING_BUFFER);

    assert(!isOverflow(buffer, 0));
    printf("pass\n");

    assert(!isOverflow(buffer, 5));
    printf("pass\n");

    assert(!isOverflow(buffer, 10));
    printf("pass\n");

    assert(isOverflow(buffer, 11));
    printf("pass\n");

    for(int i = 0; i < 3; i++){
        addFloatToRingBuffer(buffer, 0);
    }

    assert(!isOverflow(buffer, 0));
    printf("pass\n");

    assert(!isOverflow(buffer, 7));
    printf("pass\n");

    assert(isOverflow(buffer, 8));
    printf("pass\n");

    assert(isOverflow(buffer, 10));
    printf("pass\n");




    freeRingBuffer(buffer);
}

