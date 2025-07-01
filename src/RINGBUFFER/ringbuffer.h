#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "../globaldefinition.h"


struct ring_buffer{
    float * memory;
    size_t size;
    size_t write;
};


//GLOBAL RING BUFFER FUNCTIONS
struct ring_buffer * initRingBuffer(size_t size);

void freeRingBuffer(struct ring_buffer * buffer);

//TODO : ringbuffer add data (need to think if we need two functions for adding just 1 value and adding a full chunk of data or combine both, would tend more towards 2 functions, would be simpler)

//TODO : ringbuffer get full buffer (for internal ring buffer operation)



#endif

