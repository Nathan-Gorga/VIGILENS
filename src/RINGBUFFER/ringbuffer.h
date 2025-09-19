#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "../globaldefinition.h"
#include "../EVENTDATASTRUCTURES/eventdatastructure.h"
#include "../THREADFUNCTIONS/INTAKE/dataintake.h"


// these types are to ensure that certain ring buffer function 
// are only accessible to one and not the other
enum RING_BUFFER_TYPE{
    INTERNAL_RING_BUFFER,
    EVENT_RING_BUFFER,
    NUM_RING_BUFFER_TYPES
};

struct ring_buffer{

    double * memory; // the actual data is contained here
    size_t size; // size of the memory
    size_t write; // the write index of the buffer, new data is added at this position
    enum RING_BUFFER_TYPE type; 
};

struct ring_buffer * initRingBuffer(const size_t size, const enum RING_BUFFER_TYPE type);

void freeRingBuffer(struct ring_buffer * buffer);

size_t writeIndexAfterIncrement(const struct ring_buffer * buffer);

size_t writeIndexAfterDecrement(const struct ring_buffer * buffer);

size_t writeIndexAfterAddingX(const struct ring_buffer * buffer, const size_t x);

size_t writeIndexAfterSubtractingX(const struct ring_buffer * buffer, const size_t x);

void extractBufferFromRingBuffer(struct ring_buffer * buffer, double * restrict data, const size_t size_data, const size_t start, const size_t stop);

size_t numElementsBetweenIndexes(const size_t buffer_size, const size_t start, const size_t stop);

void addFloatToRingBuffer(struct ring_buffer * restrict buffer, const double data); 
 
void addBufferToRingBuffer(struct ring_buffer * buffer, const double * restrict data, const size_t size);

//TODO : find better name
size_t minusTail(const int tail, const int num_data_points);

//TODO : find better name
size_t addTail(const int tail, const int num_data_points);

#endif

