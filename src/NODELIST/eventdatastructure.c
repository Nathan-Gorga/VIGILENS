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


static head_node * initList(void){

    head_node * head = (head_node *)malloc(sizeof(head_node));

    if(head == NULL) return NULL;

    head->next = NULL;

    return head;

}


static void freeList(head_node * head){

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
