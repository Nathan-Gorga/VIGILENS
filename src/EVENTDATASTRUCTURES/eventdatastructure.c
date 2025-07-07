#include "eventdatastructure.h"


pthread_mutex_t event_ring_buffer_mutex;

pthread_mutex_t write_index_mutex; //this one is just to prevent double lock

pthread_mutex_t head_mutex;

static node * initNode(const size_t start, const size_t stop){//DONTTOUCH
    PRINTF_DEBUG

    node * n = (node *)malloc(sizeof(node));
    PRINTF_DEBUG

    if(n == NULL) return NULL;
    PRINTF_DEBUG

    n->next = NULL;
    PRINTF_DEBUG

    n->start = start;

    n->stop = stop;
    PRINTF_DEBUG

    return n;
}


static void freeNode(node * n){//DONTTOUCH
    
    assert(n != NULL);
    PRINTF_DEBUG

    free(n);
}



static void initList(void){//DONTTOUCH
    PRINTF_DEBUG

    head = (head_node *)malloc(sizeof(head_node));
    PRINTF_DEBUG

    if(head == NULL) exit(EXIT_FAILURE);
    PRINTF_DEBUG

    head->next = NULL;
}



static void freeList(void){//DONTTOUCH
    PRINTF_DEBUG

    assert(head != NULL);
    PRINTF_DEBUG

    node * curr = head->next;
    PRINTF_DEBUG

    while(curr != NULL){
        PRINTF_DEBUG

        node * next = curr->next;
        PRINTF_DEBUG

        freeNode(curr);
        PRINTF_DEBUG

        curr = next;
    }
    PRINTF_DEBUG

    free(head);
}


static void initEventRingBuffer(const size_t size_buffer){//DONTTOUCH
    PRINTF_DEBUG

    assert(size_buffer > 0);
    PRINTF_DEBUG

    event_ring_buffer = initRingBuffer(size_buffer, EVENT_RING_BUFFER);
    PRINTF_DEBUG

    if(event_ring_buffer == NULL) exit(EXIT_FAILURE);

}



static void freeEventRingBuffer(void){//DONTTOUCH
    PRINTF_DEBUG

    assert(event_ring_buffer != NULL);
    PRINTF_DEBUG

    freeRingBuffer(event_ring_buffer);
}




static void addNodeToList(node * n){//DONTTOUCH
    PRINTF_DEBUG

    assert(head != NULL);
    PRINTF_DEBUG

    assert(n != NULL);
    PRINTF_DEBUG

    if(head->next == NULL){
        head->next = n;
        return;
    }
    PRINTF_DEBUG

    node * curr = head->next;
    PRINTF_DEBUG

    while(curr->next != NULL){

        curr = curr->next;
    }
    PRINTF_DEBUG

    curr->next = n;
}



static void popNodeFromList(void){//DONTTOUCH
    PRINTF_DEBUG

    assert(head != NULL);
    PRINTF_DEBUG

    node * curr = head->next;
    PRINTF_DEBUG

    if(curr == NULL) return;
    PRINTF_DEBUG

    head->next = curr->next;
    PRINTF_DEBUG

    freeNode(curr);
}



static size_t _getEvent(float * data){//DONTTOUCH
    PRINTF_DEBUG

    assert(head != NULL);
    PRINTF_DEBUG
    
    if(head->next == NULL) return 0;//list is empty

    PRINTF_DEBUG
    node * event = head->next;
    PRINTF_DEBUG

    const size_t start = event->start;

    const size_t stop = event->stop;
    PRINTF_DEBUG

    const size_t size_data = numElementsBetweenIndexes(event_ring_buffer->size, start, stop);    
    PRINTF_DEBUG
    
    MUTEX_LOCK(&event_ring_buffer_mutex);
        
        extractBufferFromRingBuffer(event_ring_buffer, data, size_data, start, stop);

    MUTEX_UNLOCK(&event_ring_buffer_mutex);
    PRINTF_DEBUG

    popNodeFromList();
    
    PRINTF_DEBUG

    return size_data;
}


static void _addEvent(const float * data, const size_t size_data){//DONTTOUCH

    assert(data != NULL);

    assert(size_data > 0);

    assert(head != NULL);
    PRINTF_DEBUG

    
    MUTEX_LOCK(&event_ring_buffer_mutex);
    PRINTF_DEBUG

        const size_t start = event_ring_buffer->write;
        
        addBufferToRingBuffer(event_ring_buffer,data, size_data);

        const int test_stop = event_ring_buffer->write - 1;
        
        const size_t stop = test_stop >= 0 ? test_stop : event_ring_buffer->size - 1;
        PRINTF_DEBUG

    MUTEX_UNLOCK(&event_ring_buffer_mutex);
    PRINTF_DEBUG

    node * n = initNode(start, stop);
    PRINTF_DEBUG

    addNodeToList(n);
    PRINTF_DEBUG

}


void initEventDatastructure(const size_t size_buffer){//DONTTOUCH

    assert(size_buffer > 0);
    PRINTF_DEBUG

    initEventRingBuffer(size_buffer);
    PRINTF_DEBUG

    initList();
}


void freeEventDatastructure(void){//DONTTOUCH
    PRINTF_DEBUG

    freeEventRingBuffer();

    PRINTF_DEBUG
    freeList();
}




size_t getEvent(float * data){//DONTTOUCH
    
    MUTEX_LOCK(&head_mutex);

        size_t ret = _getEvent(data);

    MUTEX_UNLOCK(&head_mutex);
    
    return ret;
}


void addEvent(const float * data, const size_t size_data){//DONTTOUCH
    
    MUTEX_LOCK(&head_mutex);
        PRINTF_DEBUG
        _addEvent(data, size_data);
    
    MUTEX_UNLOCK(&head_mutex);
    PRINTF_DEBUG
    
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

    if(pthread_mutex_destroy(&ready_cond) == -1) return -1;
  
    return 0;
}


void printNodeList(void){
    node * curr = head->next;
    int i = 1;
    while(curr != NULL){
        printf("start : %zu, stop : %zu\n", curr->start, curr->stop);
        i++;
        curr = curr->next;
    }
    printf("%d nodes (including the head)\n",i);

}

void testEventDatastructure(void){
    PRINTF_DEBUG

    printNodeList();
    PRINTF_DEBUG

    // for(int i = 0; i < event_ring_buffer->size; i++){
    //     printf("%f\n", event_ring_buffer->memory[i]);
    // }
}