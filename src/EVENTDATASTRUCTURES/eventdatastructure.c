#include "eventdatastructure.h"


pthread_mutex_t event_ring_buffer_mutex;

pthread_mutex_t write_index_mutex; //this one is just to prevent double lock

pthread_mutex_t head_mutex;

static node * initNode(const size_t start, const size_t stop){

    node * n = (node *)malloc(sizeof(node));

    if(n == NULL) return NULL;

    n->next = NULL;

    n->start = start;

    n->stop = stop;

    return n;
}


static void freeNode(node * n){
    
    assert(n != NULL);

    free(n);
}



static void initList(void){

    head = (head_node *)malloc(sizeof(head_node));

    if(head == NULL) exit(EXIT_FAILURE);

    head->next = NULL;
}



static void freeList(void){

    assert(head != NULL);

    node * curr = head->next;

    while(curr != NULL){

        node * next = curr->next;

        freeNode(curr);

        curr = next;
    }

    free(head);
}


static void initEventRingBuffer(const size_t size_buffer){

    assert(size_buffer > 0);

    event_ring_buffer = initRingBuffer(size_buffer, EVENT_RING_BUFFER);

    if(event_ring_buffer == NULL) exit(EXIT_FAILURE);

}



static void freeEventRingBuffer(void){

    assert(event_ring_buffer != NULL);

    freeRingBuffer(event_ring_buffer);
}



static void _addNodeToList(node * n){

    assert(head != NULL);

    assert(n != NULL);

    if(head->next == NULL){
        head->next = n;
        return;
    }

    node * curr = head->next;

    while(curr->next != NULL){

        curr = curr->next;
    }

    curr->next = n;
}




static void _popNodeFromList(void){

    assert(head != NULL);

    node * curr = head->next;

    if(curr == NULL) return;

    head->next = curr->next;

    freeNode(curr);
}



static float * _getEvent(size_t * size_ptr){//FIXME : send as argument an int pointer for size and malloc the relevant size directly inside this function, needs to be freed after, still not great but a bit better

    assert(head != NULL);
    
    if(head->next == NULL){// list is empty
        *size_ptr = 0; 
        return NULL; 
    } 
    PRINTF_DEBUG
    node * event = head->next;
    PRINTF_DEBUG

    const size_t start = event->start;

    const size_t stop = event->stop;
    PRINTF_DEBUG

    *size_ptr = numElementsBetweenIndexes(event_ring_buffer->size, start, stop);    
    printf("size : %d\n", *size_ptr);
    PRINTF_DEBUG
    float * data = (float *)calloc(*size_ptr, sizeof(float));//FIXME : dangerous and inefficient, find a better way
    PRINTF_DEBUG

    if(data == NULL){
        *size_ptr = 0;
        return NULL; //something went wrong
    } 
    PRINTF_DEBUG

    
    MUTEX_LOCK(&event_ring_buffer_mutex);
        
        extractBufferFromRingBuffer(event_ring_buffer, data, *size_ptr, start, stop);

    MUTEX_UNLOCK(&event_ring_buffer_mutex);
    PRINTF_DEBUG

    popNodeFromList();
    PRINTF_DEBUG

    return data;
}


static void _addEvent(const float * data, const size_t size_data){

    assert(data != NULL);

    assert(size_data > 0);

    assert(head != NULL);
    PRINTF_DEBUG

    
    MUTEX_LOCK(&event_ring_buffer_mutex);
    PRINTF_DEBUG

        const size_t start = event_ring_buffer->write;
        
        addBufferToRingBuffer(event_ring_buffer,data, size_data);

        const size_t test_stop = event_ring_buffer->write - 1;
        
        const size_t stop = test_stop >= 0 ? test_stop : event_ring_buffer->size;//TODO : make a function to get the write index IN MUTEX
        PRINTF_DEBUG

    MUTEX_UNLOCK(&event_ring_buffer_mutex);
    PRINTF_DEBUG

    node * n = initNode(start, stop);
    PRINTF_DEBUG

    addNodeToList(n);
    PRINTF_DEBUG

}


void initEventDatastructure(const size_t size_buffer){

    assert(size_buffer > 0);

    initEventRingBuffer(size_buffer);

    initList();
}


void freeEventDatastructure(void){

    freeEventRingBuffer();

    freeList();
}


void addNodeToList(node * n){
    PRINTF_DEBUG
    
    // MUTEX_LOCK(&head_mutex); //FIXME : mutexes here may not be useful
    PRINTF_DEBUG

        _addNodeToList(n);
        PRINTF_DEBUG

    // MUTEX_UNLOCK(&head_mutex);
    PRINTF_DEBUG
    
}


void popNodeFromList(void){
    
    //MUTEX_LOCK(&head_mutex);//FIXME : mutexes here may not be useful
    
        _popNodeFromList();

    //MUTEX_UNLOCK(&head_mutex);
}



float * getEvent(size_t * size_ptr){
    
    MUTEX_LOCK(&head_mutex);

        float * event = _getEvent(size_ptr);

    MUTEX_UNLOCK(&head_mutex);
    
    return event;
}


void addEvent(const float * data, const size_t size_data){
    
    MUTEX_LOCK(&head_mutex);
        PRINTF_DEBUG
        _addEvent(data, size_data);
    
    MUTEX_UNLOCK(&head_mutex);
    PRINTF_DEBUG
    
}

int createMutexes(void){
    
    if(pthread_mutex_init(&head_mutex, NULL) == -1) return -1;

    if(pthread_mutex_init(&event_ring_buffer_mutex, NULL) == -1) return -1;

    if(pthread_mutex_init(&write_index_mutex, NULL) == -1) return -1;
    
    return 0;
}

int destroyMutexes(void){

    if(pthread_mutex_destroy(&head_mutex) == -1) return -1;

    if(pthread_mutex_destroy(&event_ring_buffer_mutex) == -1) return -1;

    if(pthread_mutex_destroy(&write_index_mutex) == -1) return -1;

    if(pthread_mutex_destroy(&ready_lock) == -1) return -1;

    if(pthread_mutex_destroy(&ready_cond) == -1) return -1;
  
    return 0;
}


void printNodeList(void){
    node * curr = head->next;

    while(curr != NULL){
        printf("start : %zu, stop : %zu\n", curr->start, curr->stop);
        curr = curr->next;
    }
}

void testEventDatastructure(void){
    printNodeList();

    for(int i = 0; i < event_ring_buffer->size; i++){
        printf("%f\n", event_ring_buffer->memory[i]);
    }
}