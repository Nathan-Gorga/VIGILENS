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
    
    #ifdef ASSERT_ENABLED
    
        assert(n != NULL);

    #endif

    free(n);
}



static void initList(void){

    head = (head_node *)malloc(sizeof(head_node));

    if(head == NULL) exit(EXIT_FAILURE);

    head->next = NULL;
}



static void freeList(void){
    
    if(head == NULL) return;

    node * curr = head->next;

    while(curr != NULL){

        node * next = curr->next;

        freeNode(curr);

        curr = next;
    }

    free(head);
}


static void initEventRingBuffer(const size_t size_buffer){

    #ifdef ASSERT_ENABLED

        assert(size_buffer > 0);

    #endif

    event_ring_buffer = initRingBuffer(size_buffer, EVENT_RING_BUFFER);

    if(event_ring_buffer == NULL) exit(EXIT_FAILURE);

}



static void freeEventRingBuffer(void){
    
    if(event_ring_buffer != NULL) freeRingBuffer(event_ring_buffer);
}




static void addNodeToList(node * restrict n){

    #ifdef ASSERT_ENABLED

        assert(head != NULL);

        assert(n != NULL);

    #endif

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



static void popNodeFromList(void){

    #ifdef ASSERT_ENABLED

        assert(head != NULL);

    #endif

    node * curr = head->next;

    if(curr == NULL) return;

    head->next = curr->next;

    freeNode(curr);
}



static size_t _getEvent(float * restrict data){
    
    #ifdef ASSERT_ENABLED

        assert(head != NULL);

    #endif
    
    if(head->next == NULL) return 0;//list is empty

    node * event = head->next;

    const size_t start = event->start;

    const size_t stop = event->stop;

    const size_t size_data = numElementsBetweenIndexes(event_ring_buffer->size, start, stop);     
    
    MUTEX_LOCK(&event_ring_buffer_mutex);
        
        extractBufferFromRingBuffer(event_ring_buffer, data, size_data, start, stop);

    MUTEX_UNLOCK(&event_ring_buffer_mutex);

    popNodeFromList();
    
    return size_data;
}


static void _addEvent(const float * restrict data, const size_t size_data){
   // printf(RED"adding event\n"RESET);

    #ifdef ASSERT_ENABLED

        assert(data != NULL);

        assert(size_data > 0);

        assert(head != NULL);

    #endif
    
    MUTEX_LOCK(&event_ring_buffer_mutex);

        const size_t start = event_ring_buffer->write;
        
        addBufferToRingBuffer(event_ring_buffer,data, size_data);

        const int test_stop = event_ring_buffer->write - 1;
        
        const size_t stop = test_stop >= 0 ? test_stop : event_ring_buffer->size - 1;

    MUTEX_UNLOCK(&event_ring_buffer_mutex);

    //printf("size_data = %d, start = %d, stop = %d\n", size_data, start, stop);

    node * n = initNode(start, stop);

    addNodeToList(n);
}


void initEventDatastructure(const size_t size_buffer){

    #ifdef ASSERT_ENABLED

        assert(size_buffer > 0);

    #endif

    initEventRingBuffer(size_buffer);

    initList();
}


void freeEventDatastructure(void){

    freeEventRingBuffer();

    freeList();
}




size_t getEvent(float * restrict data){
    
    MUTEX_LOCK(&head_mutex);

        size_t ret = _getEvent(data);

    MUTEX_UNLOCK(&head_mutex);
    
    return ret;
}


void addEvent(const float * restrict data, const size_t size_data){
    
    MUTEX_LOCK(&head_mutex);

        _addEvent(data, size_data);
    
    MUTEX_UNLOCK(&head_mutex);
    
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
    
    int i = 1;
    
    while(curr != NULL){
    
        printf("start : %zu, stop : %zu\n", curr->start, curr->stop);
    
        i++;
    
        curr = curr->next;
    }
    
    printf("%d nodes (including the head)\n",i);

}
