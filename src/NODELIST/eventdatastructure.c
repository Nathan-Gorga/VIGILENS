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