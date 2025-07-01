#include "ringbuffer.h"

struct ring_buffer * initRingBuffer(const size_t size){
    
    if(size <= 0) return NULL;

    struct ring_buffer * buffer = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));

    buffer->memory = (float*)calloc(size, sizeof(float));

    buffer->size = size;

    buffer->write = 0;

    return buffer;
}



