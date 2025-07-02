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

    if(buffer->memory == NULL) goto freebuffer;

    free(buffer->memory);

    freebuffer:

    free(buffer);
}

static bool isOverflow(struct ring_buffer * buffer, const size_t size_to_add){
    return (buffer->write + size_to_add) > buffer->size;
}


static inline void writeIndexIncrement(struct ring_buffer * buffer){
    assert(buffer != NULL);
    
    buffer->write++;

    //this is faster than the modulo operation in our case
    if(buffer->write == buffer->size) buffer->write = 0;
}




size_t numElementsBetweenIndexes(const size_t buffer_size, const size_t start, const size_t stop){
    assert(buffer_size > 0);

    assert(start >= 0 && stop >= 0);

    assert(start < buffer_size && stop < buffer_size);


    if(stop > start) return stop - start + 1;

    return buffer_size - start + stop + 1;
}





void extractBufferFromRingBuffer(struct ring_buffer * buffer, float * data, const size_t size_data, const size_t start, const size_t stop){
    
    assert(buffer != NULL);

    assert(data != NULL);

    const bool overflow = start > stop;
    
    size_t size;

    if(!overflow) size = stop + 1 - start;

    else size = buffer->size - start + stop + 1;
    
    assert(size == size_data);

    assert(size_data <= buffer->size);

    assert(start >= 0 && stop >= 0);

    assert(start < buffer->size && stop <buffer->size);

    if(!overflow){

        for(size_t i = 0; i < size; i ++){

            data[i] = buffer->memory[i + start];

        }

    } else {
        
        const size_t offset = buffer->size - start;

        for(size_t i = 0; i < offset; i ++){

            data[i] = buffer->memory[i + start];

        }

        for(size_t i = 0; i < stop + 1; i ++){

            data[i + offset] = buffer->memory[i];

        }
    }
}




void addFloatToRingBuffer(struct ring_buffer * buffer, const float data){
    
    assert(buffer != NULL);

    assert(buffer->type == INTERNAL_RING_BUFFER);

    buffer->memory[buffer->write] = data;

    writeIndexIncrement(buffer);
}



void addBufferToRingBuffer(struct ring_buffer * buffer, const float * data, const size_t size){

    assert(buffer != NULL);

    assert(data != NULL);

    assert(size > 0);

    assert(buffer->type == EVENT_RING_BUFFER);


    if(!isOverflow(buffer, size - 1)){
        
        for(size_t i = 0; i < size; i++){

            buffer->memory[buffer->write] = data[i];

            writeIndexIncrement(buffer);
        }

    } else {

        const size_t first_part_size = buffer->size - buffer->write;

        for(size_t i = 0; i < first_part_size; i++){

            buffer->memory[buffer->write] = data[i];

            writeIndexIncrement(buffer);
        }

        const size_t second_part_size = (size - first_part_size);

        for(size_t i = 0; i < second_part_size; i++){

            buffer->memory[buffer->write] = data[i + first_part_size];

            writeIndexIncrement(buffer);
        }
    }
}