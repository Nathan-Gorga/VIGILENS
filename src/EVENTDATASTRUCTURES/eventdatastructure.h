#ifndef EVENTDATASTRUCTURE_H
#define EVENTDATASTRUCTURE_H

#include "../globaldefinition.h"
#include "../RINGBUFFER/ringbuffer.h"

#define SAMPLING_RATE 250
#define NUM_CHANNELS 2
#define TIME_IN_BUFFER 60

#define EVENT_RING_BUFFER_SIZE (size_t)(TIME_IN_BUFFER * SAMPLING_RATE * NUM_CHANNELS)

#define MAX_EVENT_SIZE (size_t)(NUM_CHANNELS * SAMPLING_RATE * 2/*seconds*/)

typedef struct node {
    struct node * next;
    size_t start;
    size_t stop;
}node;

typedef struct{
    node * next;
}head_node;

volatile static struct ring_buffer * event_ring_buffer;

volatile static head_node * head;

extern pthread_mutex_t event_ring_buffer_mutex;

extern pthread_mutex_t write_index_mutex; //this one is just to prevent double lock

extern pthread_mutex_t head_mutex;

extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;


//GLOBAL FUNCTIONS, called only once by main, don't need mutex

/**
 * @brief Initializes the event data structure.
 *
 * @param size_buffer The size of the event ring buffer to create.
 *
 * @details This function initializes the event ring buffer with the given size and the linked list of nodes.
 *
 * @pre size_buffer must be greater than 0.
 *
 * @post event_ring_buffer is initialized with the given size and the linked list of nodes is initialized.
 */void initEventDatastructure(const size_t size_buffer);


/**
 * @brief Frees the memory of the event data structure.
 *
 * @details This function calls freeEventRingBuffer and freeList to free the memory allocated for the event ring buffer and the linked list of nodes respectively.
 *
 * @pre The event data structure must have been initialized with initEventDatastructure.
 *
 * @post All memory allocated for the event ring buffer and the linked list of nodes is freed.
 */void freeEventDatastructure(void);


 //MUTEX FUNCTIONS

int createMutexes(void);

int destroyMutexes(void);



/**
 * @brief Retrieves the next event from the event list and stores it in the provided buffer.
 *
 * @param data A pointer to a float pointer, which will be set to point to a buffer of floats containing the event.
 *
 * @return The size of the event in floats, or 0 if the event list is empty, or -1 if a memory allocation failure occurs.
 *
 * @details This function retrieves the next event from the event list and stores it in the provided buffer.
 *          Safe for multithreading
 *
 * @pre data must not be NULL.
 *
 * @post head->next points to the next node in the list, or NULL if the list is empty.
 */size_t getEvent(float * data);


/**
 * @brief Adds an event to the event ring buffer and creates a corresponding node.
 *
 * @param data The buffer of floats representing the event to add.
 * @param size_data The number of floats in the event.
 *
 * @details This function locks the event ring buffer and the head of the node list,
 *          then adds the provided event data to the event ring buffer and creates
 *          a node pointing to the event's location in the buffer. The locks are
 *          released after the operation.
 *
 * @pre data must not be NULL, size_data must be greater than 0, and the event data
 *      structure must be initialized.
 */void addEvent(const float * data, const size_t size_data);


#endif

