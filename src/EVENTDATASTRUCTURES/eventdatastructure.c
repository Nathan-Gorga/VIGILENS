#include "eventdatastructure.h"

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

//TODO : add in function comment : it is your responsability to free the data buffer
static size_t _getEvent(float ** data){

    assert(head != NULL);
    
    
    PRINTF_DEBUG


    if(head->next == NULL) return 0; // list is empty

    node * event = head->next;//TODO : make a mutex function to get elements from head

    PRINTF_DEBUG


    const size_t start = event->start;

    const size_t stop = event->stop;

    PRINTF_DEBUG


    const size_t size = numElementsBetweenIndexes(event_ring_buffer->size, start, stop);

    PRINTF_DEBUG

    *data = (float *)calloc(size, sizeof(float));

    if(*data == NULL) return -1; //something went wrong

    PRINTF_DEBUG


    extractBufferFromRingBuffer(event_ring_buffer, *data, size, start, stop);

    PRINTF_DEBUG


    popNodeFromList();

    PRINTF_DEBUG


    return size;
}


static void _addEvent(const float * data, const size_t size_data){//TESTME

    assert(data != NULL);

    assert(size_data > 0);

    assert(head != NULL);

    
    const size_t start = event_ring_buffer->write;

    addBufferToRingBuffer(event_ring_buffer,data, size_data);

    const size_t test_stop = event_ring_buffer->write - 1;

    const size_t stop = test_stop >= 0 ? test_stop : event_ring_buffer->size;//TODO : make a function to get the write index IN MUTEX

    node * n = initNode(start, stop);

    addNodeToList(n);
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
    //TODO : lock head
    _addNodeToList(n);
    //TODO : unlock head
}


void popNodeFromList(void){
    //TODO : lock head
    _popNodeFromList();
    //TODO : unlock head
}

size_t getEvent(float ** data){
    //TODO : lock head
    //TODO : lock event ring buffer

    const size_t size = _getEvent(data);
    //TODO : unlock event ring buffer
    //TODO : unlock head

    return size;
}

void addEvent(const float * data, const size_t size_data){

    //TODO : lock head
    //TODO : lock event ring buffer
    _addEvent(data, size_data);
    //TODO : unlock event ring buffer
    //TODO : unlock head
}




void test(void){
    initEventDatastructure(100);

    float data[] = {1,2,3,4,5,6,7,8,9,10};

    addEvent(data, 10);

    float data2[] = {11,12,13,14,15,16,17,18,19,20};

    addEvent(data2, 10);


    float *data3 = NULL;
    const size_t size3 = getEvent(&data3);
    
    
    PRINTF_DEBUG
    
    for(int i =0; i < size3; i++){
        printf("%f\n", data3[i]);
    }
    PRINTF_DEBUG

    free(data3);

    float *data4 = NULL;
    const size_t size4 = getEvent(&data4);

    for(int i =0; i < size4; i++){
        printf("%f\n", data4[i]);
    }

    free(data4);

    printf("%p\n",head->next);

    freeEventDatastructure();
}