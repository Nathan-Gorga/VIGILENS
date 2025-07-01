#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "../globaldefinition.h"


struct ring_buffer{
    float * memory;
    size_t size;
    size_t write;
};


//GLOBAL RING BUFFER FUNCTIONS

/**
 * @brief Initializes a ring buffer of floats with a given size.
 *
 * @param size The size of the buffer in floats.
 *
 * @return A pointer to the newly allocated ring buffer, or NULL if size is less than or equal to 0.
 *
 * @details The buffer is allocated with malloc, and the memory is initialized with calloc.
 *          The write index is set to 0.
 */
struct ring_buffer * initRingBuffer(const size_t size);

void freeRingBuffer(struct ring_buffer * buffer);

//TODO : ringbuffer add data (need to think if we need two functions for adding just 1 value and adding a full chunk of data or combine both, would tend more towards 2 functions, would be simpler)

//TODO : ringbuffer get full buffer (for internal ring buffer operation)


#endif

