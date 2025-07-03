#ifndef EVENTDATASTRUCTURE_H
#define EVENTDATASTRUCTURE_H

#include "../globaldefinition.h"
#include "../RINGBUFFER/ringbuffer.h"

#define SAMPLING_RATE 200
#define NUM_CHANNELS 2
#define TIME_IN_BUFFER 60

#define EVENT_RING_BUFFER_SIZE (size_t)(TIME_IN_BUFFER * SAMPLING_RATE * NUM_CHANNELS)


typedef struct node{
    struct node * next;
    size_t start;
    size_t stop;
}node;

typedef struct head_node{
    struct node * next;
}head_node;

volatile static struct ring_buffer * event_ring_buffer;

volatile static head_node * head;

extern pthread_mutex_t event_ring_buffer_mutex;

extern pthread_mutex_t write_index_mutex; //this one is just to prevent double lock

extern pthread_mutex_t head_mutex;

extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;

//STATIC INITS AND FREES

/**
 * @brief Initializes a node with a given start and stop index.
 *
 * @param start The start index of the node.
 * @param stop The stop index of the node.
 *
 * @return A newly allocated node with the given start and stop indices, or NULL if a memory allocation failure occurs.
 *
 * @details This function allocates memory for a node structure and initializes its members.
 *          The node's next pointer is set to NULL, and its start and stop indices are set to the given values.
 */static node * initNode(const size_t start, const size_t stop);


/**
 * @brief Frees the memory allocated for a node.
 *
 * @param n The node to free.
 *
 * @details This function frees the memory allocated for a node.
 *
 * @pre n must not be NULL.
 */static void freeNode(node * n);


/**
 * @brief Initializes the head node of the linked list.
 *
 * @details Allocates memory for a head node and initializes its members.
 *          The head node's next pointer is set to NULL.
 */static void initList(void);


/**
 * @brief Frees the memory of a linked list of nodes.
 *
 * @details This function is called when the linked list of nodes is no longer needed.
 *          It is the user's responsability to call this function when the linked list is no longer needed.
 */static void freeList(void);


/**
 * @brief Initializes the event ring buffer.
 *
 * @param size_buffer The size of the event ring buffer to create.
 *
 * @details This function initializes the event ring buffer with the given size.
 *          It asserts that the given size is greater than 0.
 *          It also asserts that the event ring buffer is not already initialized.
 *
 * @pre size_buffer must be greater than 0.
 *
 * @post event_ring_buffer is initialized with the given size.
 */static void initEventRingBuffer(const size_t size_buffer);


/**
 * @brief Frees the memory of the event ring buffer.
 *
 * @details This function is called when the event ring buffer is no longer needed.
 *          It is the user's responsability to call this function when the event ring buffer is no longer needed.
 */static void freeEventRingBuffer(void);


//STATIC EVENT RING BUFFER MANIPULATION

/**
 * @brief Adds a node to the list.
 *
 * @param n The node to add to the list.
 *
 * @details This function adds a node to the end of the list.
 *
 * @pre head must not be NULL, and n must not be NULL.
 */static void _addNodeToList(node * n);


/**
 * @brief Pops a node from the list.
 *
 * @details This function frees the head's next node and moves the head's next pointer to the next node.
 *
 * @pre head must not be NULL.
 */static void _popNodeFromList(void);


/**
 * @brief Retrieves the next event from the event list and stores it in the provided buffer.
 *
 * @param data A pointer to a float pointer, which will be set to point to a buffer of floats containing the event.
 *
 * @return The size of the event in floats, or 0 if the event list is empty, or -1 if a memory allocation failure occurs.
 *
 * @details This function retrieves the next event from the event list and stores it in the provided buffer.
 *          IT IS YOUR RESPONSABILITY TO FREE DATA AFTER USE
 *
 * @pre head must not be NULL.
 */static float * _getEvent(size_t * size_ptr);//TODO : New function comment



/**
 * @brief Adds an event to the event ring buffer and creates a node for it.
 *
 * @param data The buffer of floats representing the event to add.
 * @param size_data The number of floats in the event.
 *
 * @details This function adds the given event data to the event ring buffer.
 *
 * @pre data is not NULL, size_data is greater than 0, and head is not NULL.
 */static void _addEvent(const float * data, const size_t size_data);


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
 * @brief Adds a node to the list.
 *
 * @param n The node to add to the list.
 *
 * @details This function adds a node to the end of the list.
 *          Safe for multithreading.
 *
 * @pre n must not be NULL.
 *
 * @post head->next is set to n, and n->next is set to NULL.
 */void addNodeToList(node * n);


/**
 * @brief Pops the next node from the list.
 *
 * @details This function removes the node immediately following the head
 *          in a thread-safe manner by using a mutex.
 *
 * @pre The head node must not be NULL.
 *
 * @post The list's head now points to the second node, and the memory of
 *       the removed node is freed.
 */void popNodeFromList(void);


/**
 * @brief Retrieves the next event from the event list and stores it in the provided buffer.
 *
 * @param data A pointer to a float pointer, which will be set to point to a buffer of floats containing the event.
 *
 * @return The size of the event in floats, or 0 if the event list is empty, or -1 if a memory allocation failure occurs.
 *
 * @details This function retrieves the next event from the event list and stores it in the provided buffer.
 *          IT IS YOUR RESPONSABILITY TO FREE DATA AFTER USE
 *
 * @pre head must not be NULL.
 */float * getEvent(size_t * size_ptr);//TODO : New function comment


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



void testEventDatastructure(void);

#endif

