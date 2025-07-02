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