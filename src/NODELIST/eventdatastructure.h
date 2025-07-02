#ifndef NODELIST_H
#define NODELIST_H

#include "../globaldefinition.h"

static struct ring_buffer event_ring_buffer;

typedef struct node{
    struct node * next;
    size_t start;
    size_t stop;
}node;


//STATIC INITS AND FREES

static node * initNode(const size_t start, const size_t stop);

static void freeNode(node * n);

static node * initList(void);//returns head of list

static void freeList(node * head);

static void initEventRingBuffer(const size_t size_buffer);

static void freeEventRingBuffer(void);


//STATIC EVENT RING BUFFER MANIPULATION

static void _addNodeToList(node * head, node * n);

static void _popNodeFromList(node * head);

static node * _getEventFromList(node * head);

static _addEventToEventRingBuffer(const float * data, const size_t size_data);


//GLOBAL FUNCTIONS, called only once by main, don't need mutex

node * initEventDatastructure(const size_t size_buffer);

void freeEventDatastructure(node * head);


//MUTEX FUNCTIONS

void addNodeToList(node * head, node * n);

void popNodeFromList(node * head);

node * getEventFromList(node * head);

void addEventToEventRingBuffer(const float * data, const size_t size_data);

#endif

