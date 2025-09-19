#include "eventdatastructure.h"

pthread_mutex_t event_ring_buffer_mutex;

pthread_mutex_t write_index_mutex; 

pthread_mutex_t head_mutex;

static node * initNode(const size_t start, const size_t stop){//DONTTOUCH

    node * n = (node *)malloc(sizeof(node));

    if(n == NULL) return NULL;

    n->next = NULL;

    n->start = start;

    n->stop = stop;

    return n;
}

static void freeNode(node * n){//DONTTOUCH
    
    #ifdef ASSERT_ENABLED
    
        assert(n != NULL);

    #endif

    free(n);
}

static void initList(void){//DONTTOUCH

    /*
    THE START OF THE LINKED LIST SHOULD ALWAYS BE HEAD, NOT A NODE
    */

    head = (head_node *)malloc(sizeof(head_node));

    if(head == NULL) exit(EXIT_FAILURE);

    head->next = NULL;
}

static void freeList(void){//DONTTOUCH
    
    if(head == NULL) return;

    node * curr = (node *)head->next;

    while(curr != NULL){

        node * next = (node *)curr->next;

        freeNode(curr);

        curr = next;
    }

    free((void *)head);
}

static void initEventRingBuffer(const size_t size_buffer){//DONTTOUCH

    /*
    This function is a wraper of initRingBuffer, to check if the type is correct
    
    */

    #ifdef ASSERT_ENABLED

        assert(size_buffer > 0);

    #endif

    event_ring_buffer = initRingBuffer(size_buffer, EVENT_RING_BUFFER);

    if(event_ring_buffer == NULL) exit(EXIT_FAILURE);

}

static void freeEventRingBuffer(void){//DONTTOUCH

    /*
    To match the init wrapper of the event buffer
    */
    
    if(event_ring_buffer != NULL) freeRingBuffer((struct ring_buffer *)event_ring_buffer);

}

static void addNodeToList(node * n){//DONTTOUCH

    /*
    works as a FIFO linked list
    */


    #ifdef ASSERT_ENABLED

        assert(head != NULL);

        assert(n != NULL);

    #endif

    if(head->next == NULL){

        head->next = (node *)n;

        return;
    }

    node * curr = (node *)head->next;

    while(curr->next != NULL){

        curr = (node *)curr->next;
    }

    curr->next = (node *)n;
}

static void popNodeFromList(void){//DONTTOUCH

    #ifdef ASSERT_ENABLED

        assert(head != NULL);

    #endif

    node * curr = (node *)head->next;

    if(curr == NULL) return;

    head->next = curr->next;

    freeNode(curr);
}

static size_t _getEvent(double * data){
    
    /*
    This is the actual get event function, only accessible via the mutexed function wrapper
    */

    #ifdef ASSERT_ENABLED

        assert(head != NULL);

    #endif
    
    if(head->next == NULL) return 0;//list is empty

    node * event = (node *)head->next;

    const size_t start = event->start;

    const size_t stop = event->stop;

    const size_t size_data = numElementsBetweenIndexes(event_ring_buffer->size, start, stop);     

    // the head mutex is locked for the whole function, here, only the ERB mutex
    MUTEX_LOCK(&event_ring_buffer_mutex);
        
        extractBufferFromRingBuffer((struct ring_buffer *)event_ring_buffer, data, size_data, start, stop);

    MUTEX_UNLOCK(&event_ring_buffer_mutex);

    popNodeFromList();
    
    return size_data;
}

static void _addEvent(double *  data, const size_t size_data){

    /*
    this is the actual addEvent function, only accessible via the mutexed function wrapper
    */

    #ifdef ASSERT_ENABLED

        assert(data != NULL);

        assert(size_data > 0);

        assert(head != NULL);

    #endif
    
    // the head mutex is locked for the whole funciton, here, only the ERB mutex
    MUTEX_LOCK(&event_ring_buffer_mutex);

        const size_t start = event_ring_buffer->write;
        
        addBufferToRingBuffer((struct ring_buffer *)event_ring_buffer,data, size_data);

        const int test_stop = event_ring_buffer->write - 1;
        
    MUTEX_UNLOCK(&event_ring_buffer_mutex);

    const size_t stop = test_stop >= 0 ? test_stop : event_ring_buffer->size - 1;

    node * n = initNode(start, stop);

    addNodeToList(n);
}

void initEventDatastructure(const size_t size_buffer){//DONTTOUCH

    #ifdef ASSERT_ENABLED

        assert(size_buffer > 0);

    #endif

    initEventRingBuffer(size_buffer);

    initList();
}

void freeEventDatastructure(void){//DONTTOUCH

    freeEventRingBuffer();

    freeList();
}

size_t getEvent(double *  data){//DONTTOUCH
    
    /*
    this function is the outward facing wrapper, 
    you cannot access ERB without it

    this is intentional as to not cause race conditions
    */

    MUTEX_LOCK(&head_mutex);

        size_t ret = _getEvent(data);

    MUTEX_UNLOCK(&head_mutex);
    
    return ret;
}

void addEvent(double * data, const size_t size_data){//DONTTOUCH
    
    /*
    this function is the outward facing wrapper, 
    you cannot access ERB without it

    this is intentional as to not cause race conditions
    */

    MUTEX_LOCK(&head_mutex);

        _addEvent(data, size_data);
    
    MUTEX_UNLOCK(&head_mutex);
    
}

int createMutexes(void){//DONTTOUCH
    
    if(pthread_mutex_init(&head_mutex, NULL) == -1) return -1;

    if(pthread_mutex_init(&event_ring_buffer_mutex, NULL) == -1) return -1;

    if(pthread_mutex_init(&write_index_mutex, NULL) == -1) return -1;
    
    return 0;
}

int destroyMutexes(void){//DONTTOUCH

    if(pthread_mutex_destroy(&head_mutex) == -1) return -1;

    if(pthread_mutex_destroy(&event_ring_buffer_mutex) == -1) return -1;

    if(pthread_mutex_destroy(&write_index_mutex) == -1) return -1;

    if(pthread_mutex_destroy(&ready_lock) == -1) return -1;

    if(pthread_cond_destroy(&ready_cond) == -1) return -1;
  
    return 0;
}

