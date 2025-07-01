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


/**
 * @brief Checks if adding a specified amount of data to a ring buffer would overflow it.
 *
 * @param buffer The ring buffer to check.
 * @param size_to_add The amount of data to add.
 *
 * @return true if adding the specified amount of data would overflow the buffer, false otherwise.
 */bool isOverflow(struct ring_buffer * buffer, const size_t size_to_add);


/**
 * @brief Extracts a portion of data from a ring buffer into a separate buffer.
 *
 * @param buffer The ring buffer to extract data from.
 * @param data The destination buffer where the extracted data will be stored.
 * @param start The starting index in the ring buffer to begin extraction.
 * @param stop The stopping index in the ring buffer to end extraction.
 *
 * @return The size of the data buffer.
 *
 * @details This function extracts data from a ring buffer between the specified
 *          start and stop indices and stores it in the provided data buffer.
 *          It handles wrapping around the ring buffer if necessary.
 *
 * @pre buffer is not NULL, data is not NULL, and start is not equal to stop.
 */size_t extractBufferFromRingBuffer(struct ring_buffer * buffer, float * data, const size_t start, const size_t stop);




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
 
 


///EVENT RING BUFFER

/**
 * @brief Adds a buffer of floats to an event ring buffer.
 *
 * @param buffer The event ring buffer to add the buffer to.
 * @param data The buffer of floats to add.
 * @param size The size of the buffer to add.
 *
 * @return The new write index of the ring buffer.
 *
 * @details This function adds a buffer of floats to the event ring buffer.
 * 
 * @pre buffer is not NULL and buffer's type is EVENT_RING_BUFFER.
 */size_t addBufferToRingBuffer(struct ring_buffer * buffer, const float * data, const size_t size);



#endif

