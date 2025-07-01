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


/*************  ✨ Windsurf Command ⭐  *************/
/**
 * @brief Increments the write index of a ring buffer.
 *
 * @param buffer A pointer to the ring buffer whose write index is to be incremented.
 *
 * @details This function increments the write index of the given ring buffer. 
 *          If the write index reaches the buffer's size, it wraps around to 0.
 *          This operation is optimized to be faster than using the modulo operation.
 *          The function assumes that the buffer is not NULL.
 */

/*******  04366a86-36a0-4a7b-bb87-4de128c62a1b  *******/
void writeIndexIncrement(struct ring_buffer * buffer){//TESTME
    assert(buffer != NULL);
    
    buffer->write++;

    //this is faster than the modulo operation in our case
    if(buffer->write == buffer->size) buffer->write = 0;
}



//TESTME : make sure this function works for all input parameters
void addFloatToRingBuffer(struct ring_buffer * buffer, const float data){
    
    assert(buffer != NULL);

    assert(buffer->type == INTERNAL_RING_BUFFER);

    buffer->memory[buffer->write] = data;

    //FIXME : needs the write index increment function

}