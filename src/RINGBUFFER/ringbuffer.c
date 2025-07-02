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


static void writeIndexIncrement(struct ring_buffer * buffer){
    assert(buffer != NULL);
    
    buffer->write++;

    //this is faster than the modulo operation in our case
    if(buffer->write == buffer->size) buffer->write = 0;
}



size_t extractBufferFromRingBuffer(struct ring_buffer * buffer, float * data, const size_t start, const size_t stop){//TESTME : this function absolutely needs testing
    
    assert(buffer != NULL);

    assert(data != NULL);

    assert(start != stop);

    bool overflow = false;

    if(start > stop) overflow = true;

    size_t size;

    if(!overflow) size = stop - start;

    else size = buffer->size - start + stop;

    assert(sizeof(data) >= (size) * sizeof(float));

    if(!overflow){

        for(size_t i = start; i < stop; i ++){//FIXME : switch this with a memcpy

            data[i - start] = buffer->memory[i];

        }

    } else {

        for(size_t i = start; i < buffer->size; i ++){//FIXME : switch this with a memcpy

            data[i - start] = buffer->memory[i];

        }

        const size_t offset = buffer->size - start;

        for(size_t i = 0; i < stop; i ++){//FIXME : switch this with a memcpy

            data[i + offset] = buffer->memory[i];

        }
    }

    return size;
}




void addFloatToRingBuffer(struct ring_buffer * buffer, const float data){
    
    assert(buffer != NULL);

    assert(buffer->type == INTERNAL_RING_BUFFER);

    buffer->memory[buffer->write] = data;

    writeIndexIncrement(buffer);
}



size_t addBufferToRingBuffer(struct ring_buffer * buffer, const float * data, const size_t size){//TESTME : this function absolutely needs testing

    assert(buffer != NULL);

    assert(data != NULL);

    assert(size > 0);

    assert(buffer->type == EVENT_RING_BUFFER);

    bool overflow = false;

    if(isOverflow(buffer, size)) overflow = true;

    if(!overflow){

        const size_t end_of_data = buffer->write + size;

        for(size_t i = buffer->write; i < end_of_data; i++){//FIXME : switch this with a memcpy

            buffer->memory[i] = data[i];
        }

        buffer->write = end_of_data;

    } else {

        const size_t first_part_size = buffer->size;

        for(size_t i = buffer->write; i < first_part_size; i++){//FIXME : switch this with a memcpy

            buffer->memory[i] = data[i];
        }

        const size_t second_part_size = size;

        for(size_t i = 0; i < second_part_size; i++){//FIXME : switch this with a memcpy

            buffer->memory[i] = data[i + first_part_size];
        }

        buffer->write = second_part_size;
    }

    return buffer->write;
}