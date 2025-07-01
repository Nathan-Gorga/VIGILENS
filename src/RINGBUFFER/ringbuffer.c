#include "ringbuffer.h"

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
struct ring_buffer * initRingBuffer(const size_t size){
    
    if(size <= 0) return NULL;

    struct ring_buffer * buffer = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));

    buffer->memory = (float*)calloc(size, sizeof(float));

    buffer->size = size;

    buffer->write = 0;

    return buffer;
}



