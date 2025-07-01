#include "ringbuffer.h"


struct ring_buffer * initRingBuffer(const size_t size, const enum RING_BUFFER_TYPE type){
    
    if(size == 0) return NULL; //size cannot be negative (size_t is unsigned)

    struct ring_buffer * buffer = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));

    buffer->memory = (float*)calloc(size, sizeof(float));

    buffer->size = size;

    buffer->write = 0;

    buffer->type = type;

    return buffer;
}


void freeRingBuffer(struct ring_buffer * buffer){

    if(buffer == NULL) return;
    free(buffer);
    
    if(buffer->memory == NULL) return;
    free(buffer->memory);
}



static void writeIndexIncrement(struct ring_buffer * buffer){//TESTME
    assert(buffer != NULL);
    
    buffer->write++;

    //this is faster than the modulo operation in our case
    if(buffer->write == buffer->size) buffer->write = 0;
}




void addFloatToRingBuffer(struct ring_buffer * buffer, const float data){//TESTME : make sure this function works for all input parameters
    
    assert(buffer != NULL);

    assert(buffer->type == INTERNAL_RING_BUFFER);

    buffer->memory[buffer->write] = data;

    writeIndexIncrement(buffer);
}