#include "ringbuffer.h"


struct ring_buffer * initRingBuffer(const size_t size, const enum RING_BUFFER_TYPE type){
    
    if(size == 0) return NULL; //size cannot be negative (size_t is unsigned)

    struct ring_buffer * buffer = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));

    if(buffer == NULL) return NULL;

    buffer->memory = (float*)calloc(size, sizeof(float));

    if(buffer->memory == NULL) {
        free(buffer);
        return NULL;
    }

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

static bool isUnderflow(struct ring_buffer * buffer, const size_t size_to_subtract){
    const int test = buffer->write - size_to_subtract;//to avoid underflow
    return test < 0;
}


static inline size_t _writeIndexAfterIncrement(const struct ring_buffer * buffer){
    return (buffer->write + 1) % buffer->size;
}


size_t writeIndexAfterIncrement(const struct ring_buffer * buffer){

    if(buffer->type == EVENT_RING_BUFFER){
        
        MUTEX_LOCK(&write_index_mutex);

            const size_t temp = _writeIndexAfterIncrement(buffer);

        MUTEX_UNLOCK(&write_index_mutex);

        return temp;
    }

    return _writeIndexAfterIncrement(buffer);
}


static inline size_t _writeIndexAfterDecrement(const struct ring_buffer * buffer){
    return (buffer->write - 1) % buffer->size;
}


size_t writeIndexAfterDecrement(const struct ring_buffer * buffer){

    if(buffer->type == EVENT_RING_BUFFER){
        
        MUTEX_LOCK(&write_index_mutex);

            const size_t temp = _writeIndexAfterDecrement(buffer);

        MUTEX_UNLOCK(&write_index_mutex);

        return temp;
    } 
    
    return _writeIndexAfterDecrement(buffer);
    
}


static inline size_t _writeIndexAfterAddingX(const struct ring_buffer * buffer, const size_t x){//TESTME

    //the negation is for speed purposes (false is more common in our case than true)
    return !isOverflow(buffer, x) ? buffer->write + x : (x + buffer->write) - buffer->size;

}

size_t writeIndexAfterAddingX(const struct ring_buffer * buffer, const size_t x){//TESTME

    if(buffer->type == EVENT_RING_BUFFER){
        
        MUTEX_LOCK(&write_index_mutex);

            const size_t temp = _writeIndexAfterAddingX(buffer, x); 

        MUTEX_UNLOCK(&write_index_mutex);

        return temp;
    }

    return _writeIndexAfterAddingX(buffer, x);
}



static inline size_t _writeIndexAfterSubtractingX(const struct ring_buffer * buffer, const size_t x){//TESTME
    const int temp = buffer->write - x;//to avoid underflow
    return !isUnderflow(buffer, x) ? temp : temp + buffer->size;
}

size_t writeIndexAfterSubtractingX(const struct ring_buffer * buffer, const size_t x){//TESTME

    if(buffer->type == EVENT_RING_BUFFER){
        
        MUTEX_LOCK(&write_index_mutex);

            const size_t temp = _writeIndexAfterSubtractingX(buffer, x); 

        MUTEX_UNLOCK(&write_index_mutex);

        return temp;
    }

    return _writeIndexAfterSubtractingX(buffer, x);
}



static inline void _writeIndexIncrement(struct ring_buffer * buffer){

    #ifdef ASSERT_ENABLED

        assert(buffer != NULL);
    
    #endif

    buffer->write = (buffer->write + 1) % buffer->size;
}


static void writeIndexIncrement(struct ring_buffer * buffer){

    if(buffer->type == EVENT_RING_BUFFER){
        
        MUTEX_LOCK(&write_index_mutex);

            _writeIndexIncrement(buffer);

        MUTEX_UNLOCK(&write_index_mutex);       

    } else _writeIndexIncrement(buffer);
}


size_t numElementsBetweenIndexes(const size_t buffer_size, const size_t start, const size_t stop){

    #ifdef ASSERT_ENABLED
   
        assert(buffer_size > 0);

        assert(start >= 0 && stop >= 0);

        assert(start < buffer_size && stop < buffer_size); 

    #endif

    if(stop > start) return stop - start + 1;

    return buffer_size - start + stop + 1;
}


void extractBufferFromRingBuffer(const struct ring_buffer * buffer, float * restrict data, const size_t size_data, const size_t start, const size_t stop){
    
    #ifdef ASSERT_ENABLED

        assert(buffer != NULL);

        assert(data != NULL);

    #endif

    const bool overflow = start > stop;
    
    size_t size = numElementsBetweenIndexes(buffer->size, start, stop);
    
    #ifdef ASSERT_ENABLED

        assert(size == size_data);

        assert(size_data <= buffer->size);

        assert(start >= 0 && stop >= 0);

        assert(start < buffer->size && stop < buffer->size);

    #endif

    if(!overflow){

        memmove(data, buffer->memory + start, size * sizeof(float));

    } else {
        
        const size_t offset = buffer->size - start;

        memmove(data, buffer->memory + start, offset * sizeof(float));

        memmove(data + offset, buffer->memory, (stop + 1) * sizeof(float));
    }
}

void addFloatToRingBuffer(struct ring_buffer * restrict buffer, const float data){
    
    #ifdef ASSERT_ENABLED

        assert(buffer != NULL);

        assert(buffer->type == INTERNAL_RING_BUFFER);

    #endif

    buffer->memory[buffer->write] = data;// no need for mutex (internal)

    buffer->write = (buffer->write + 1) % buffer->size; //we dont use writeIndexIncrement for this one, it is only used for internal buffer and therefore we need to bypass the mutex for speed
}



void addBufferToRingBuffer(struct ring_buffer * buffer, const float * restrict data, const size_t size){

    #ifdef ASSERT_ENABLED


        assert(buffer != NULL);

        assert(data != NULL);

        assert(size > 0);

        assert(buffer->type == EVENT_RING_BUFFER);

    #endif

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
