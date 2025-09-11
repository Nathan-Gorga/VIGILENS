#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "../globaldefinition.h"
#include "../EVENTDATASTRUCTURES/eventdatastructure.h"
#include "../THREADFUNCTIONS/INTAKE/dataintake.h"

enum RING_BUFFER_TYPE{

    INTERNAL_RING_BUFFER,

    EVENT_RING_BUFFER,

    NUM_RING_BUFFER_TYPES

};

struct ring_buffer{

    float * memory;

    size_t size;

    size_t write;

    enum RING_BUFFER_TYPE type;  
    
};

struct ring_buffer * initRingBuffer(const size_t size, const enum RING_BUFFER_TYPE type);

void freeRingBuffer(struct ring_buffer * buffer);

size_t writeIndexAfterIncrement(const struct ring_buffer * buffer);

size_t writeIndexAfterDecrement(const struct ring_buffer * buffer);

size_t writeIndexAfterAddingX(const struct ring_buffer * buffer, const size_t x);

size_t writeIndexAfterSubtractingX(const struct ring_buffer * buffer, const size_t x);

void extractBufferFromRingBuffer(struct ring_buffer * buffer, float * restrict data, const size_t size_data, const size_t start, const size_t stop);

size_t numElementsBetweenIndexes(const size_t buffer_size, const size_t start, const size_t stop);

void addFloatToRingBuffer(struct ring_buffer * restrict buffer, const float data); 
 
void addBufferToRingBuffer(struct ring_buffer * buffer, const float * restrict data, const size_t size);

//TODO : find better name
size_t minusTail(const int tail, const int num_data_points);

//TODO : find better name
size_t addTail(const int tail, const int num_data_points);

#endif

