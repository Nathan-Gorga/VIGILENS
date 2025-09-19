#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "../globaldefinition.h"
#include "../EVENTDATASTRUCTURES/eventdatastructure.h"
#include "../THREADFUNCTIONS/INTAKE/dataintake.h"

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

void extractBufferFromRingBuffer(struct ring_buffer * buffer, double * restrict data, const size_t start, const size_t stop);

size_t numElementsBetweenIndexes(const size_t buffer_size, const size_t start, const size_t stop);

void addFloatToRingBuffer(struct ring_buffer * restrict buffer, const double data); 
 
void addBufferToRingBuffer(struct ring_buffer * buffer, const double * restrict data, const size_t size);

// size_t minusTail(const int tail, const int num_data_points);

// size_t addTail(const int tail, const int num_data_points);

#endif

