#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "../globaldefinition.h"

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

///GLOBAL RING BUFFER FUNCTIONS

/**
 * @brief Initializes a ring buffer of floats with a specified size and type.
 *
 * @param size The number of floats the buffer should hold.
 * @param type The type of ring buffer, either INTERNAL_RING_BUFFER or EVENT_RING_BUFFER.
 *
 * @return A pointer to the newly created ring buffer, or NULL if the specified size is 0.
 *
 * @details Allocates memory for a ring buffer structure and initializes its members.
 *          The buffer's memory is allocated using calloc, ensuring all floats are initially zero.
 *          The write index is initialized to 0 and the buffer type is set to the specified type.
 */struct ring_buffer * initRingBuffer(const size_t size, const enum RING_BUFFER_TYPE type);




/**
 * @brief Frees the memory of a ring buffer.
 *
 * @param buffer The ring buffer to be freed.
 *
 * @details This function first checks if the ring buffer is not NULL, and if so, it frees the memory allocated for it.
 *          It then checks if the memory of the ring buffer is not NULL, and if so, it frees that memory as well.
 */void freeRingBuffer(struct ring_buffer * buffer);



/**
 * @brief Increments the write index of a ring buffer.
 *
 * @param buffer A pointer to the ring buffer whose write index is to be incremented.
 *
 * @details This function increments the write index of the given ring buffer. 
 *          If the write index reaches the buffer's size, it wraps around to 0.
 *          
 */static void writeIndexIncrement(struct ring_buffer * buffer);


///INTERNAL RING BUFFER FUNCTIONS

/**
 * @brief Adds a float to an internal ring buffer.
 *
 * @param buffer The ring buffer to add the float to.
 * @param data The float to add.
 *
 * @details This function adds the given float to the internal ring buffer at the current write index. 
 *          It then increments the write index, wrapping it around to the start of the buffer if necessary.
 *
 * @pre buffer is not NULL and buffer's type is INTERNAL_RING_BUFFER.
 */void addFloatToRingBuffer(struct ring_buffer * buffer, const float data); 


//TODO : ringbuffer add data (big buffer) make sure only event buffer can get accessed through here

//TODO : ringbuffer get full buffer (for internal ring buffer operation)


#endif

