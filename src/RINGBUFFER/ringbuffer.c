#include "ringbuffer.h"


struct ring_buffer * initRingBuffer(const size_t size, const enum RING_BUFFER_TYPE type){
    PRINTF_DEBUG
    
    if(size == 0) return NULL; //size cannot be negative (size_t is unsigned)

    struct ring_buffer * buffer = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));
    PRINTF_DEBUG

    if(buffer == NULL) return NULL;
    PRINTF_DEBUG

    buffer->memory = (float*)calloc(size, sizeof(float));

    if(buffer->memory == NULL) {
        free(buffer);
        return NULL;
    }
    PRINTF_DEBUG

    PRINTF_DEBUG
    buffer->size = size;

    buffer->write = 0;

    buffer->type = type;

    return buffer;
}


void freeRingBuffer(struct ring_buffer * buffer){

    if(buffer == NULL) return;
    PRINTF_DEBUG

    if(buffer->memory == NULL) goto freebuffer;

    free(buffer->memory);
    PRINTF_DEBUG

    freebuffer:
    PRINTF_DEBUG

    free(buffer);
}

static bool isOverflow(struct ring_buffer * buffer, const size_t size_to_add){
    return (buffer->write + size_to_add) > buffer->size;
}



static inline void _writeIndexIncrement(struct ring_buffer * buffer){
    assert(buffer != NULL);
    PRINTF_DEBUG
    
    buffer->write++;

    //this is faster than the modulo operation in our case
    if(buffer->write == buffer->size) buffer->write = 0;
}


static void writeIndexIncrement(struct ring_buffer * buffer){
    PRINTF_DEBUG

    if(buffer->type == EVENT_RING_BUFFER){
        PRINTF_DEBUG

        
        MUTEX_LOCK(&write_index_mutex);
        PRINTF_DEBUG

            _writeIndexIncrement(buffer);

        MUTEX_UNLOCK(&write_index_mutex);       
        PRINTF_DEBUG

    } else _writeIndexIncrement(buffer);
}



size_t numElementsBetweenIndexes(const size_t buffer_size, const size_t start, const size_t stop){
    assert(buffer_size > 0);
    PRINTF_DEBUG

    assert(start >= 0 && stop >= 0);
    PRINTF_DEBUG

    PRINTF_DEBUG
    assert(start < buffer_size && stop < buffer_size); 

    PRINTF_DEBUG

    if(stop > start) return stop - start + 1;

    return buffer_size - start + stop + 1;
}





void extractBufferFromRingBuffer(struct ring_buffer * buffer, float * data, const size_t size_data, const size_t start, const size_t stop){
    PRINTF_DEBUG
    
    assert(buffer != NULL);

    assert(data != NULL);
    PRINTF_DEBUG

    const bool overflow = start > stop;
    PRINTF_DEBUG
    
    size_t size = numElementsBetweenIndexes(buffer->size, start, stop);
    
    assert(size == size_data);

    assert(size_data <= buffer->size);

    assert(start >= 0 && stop >= 0);

    assert(start < buffer->size && stop < buffer->size);
    PRINTF_DEBUG

    if(!overflow){
        PRINTF_DEBUG

        for(size_t i = 0; i < size; i ++){
            PRINTF_DEBUG

            data[i] = buffer->memory[i + start];
            PRINTF_DEBUG

        }

    } else {
        PRINTF_DEBUG
        
        const size_t offset = buffer->size - start;
        PRINTF_DEBUG

        for(size_t i = 0; i < offset; i ++){
            PRINTF_DEBUG

            data[i] = buffer->memory[i + start];
            PRINTF_DEBUG

        }
        PRINTF_DEBUG

        for(size_t i = 0; i < stop + 1; i ++){

            data[i + offset] = buffer->memory[i];
            PRINTF_DEBUG

        }
        PRINTF_DEBUG
    }
    PRINTF_DEBUG

}




void addFloatToRingBuffer(struct ring_buffer * buffer, const float data){
    
    assert(buffer != NULL);

    assert(buffer->type == INTERNAL_RING_BUFFER);
    PRINTF_DEBUG

    buffer->memory[buffer->write] = data;// no need for mutex (internal)
    PRINTF_DEBUG

    writeIndexIncrement(buffer);
}



void addBufferToRingBuffer(struct ring_buffer * buffer, const float * data, const size_t size){

    assert(buffer != NULL);

    assert(data != NULL);

    assert(size > 0);

    assert(buffer->type == EVENT_RING_BUFFER);

    PRINTF_DEBUG

    if(!isOverflow(buffer, size - 1)){
        
        for(size_t i = 0; i < size; i++){
            PRINTF_DEBUG

            buffer->memory[buffer->write] = data[i];
            PRINTF_DEBUG

            writeIndexIncrement(buffer);
        }

    } else {
        PRINTF_DEBUG

        const size_t first_part_size = buffer->size - buffer->write;
        PRINTF_DEBUG

        for(size_t i = 0; i < first_part_size; i++){
            PRINTF_DEBUG

            buffer->memory[buffer->write] = data[i];
            PRINTF_DEBUG

            writeIndexIncrement(buffer);
        }
        PRINTF_DEBUG

        const size_t second_part_size = (size - first_part_size);
        PRINTF_DEBUG

        for(size_t i = 0; i < second_part_size; i++){
            PRINTF_DEBUG

            buffer->memory[buffer->write] = data[i + first_part_size];

            writeIndexIncrement(buffer);
        }
    }
}