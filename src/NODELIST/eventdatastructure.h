#ifndef NODELIST_H
#define NODELIST_H

#include "../globaldefinition.h"
#include "../RINGBUFFER/ringbuffer.h"

static struct ring_buffer * event_ring_buffer;

typedef struct node{
    struct node * next;
    size_t start;
    size_t stop;
}node;

typedef struct head_node{
    struct node * next;
}head_node;


//STATIC INITS AND FREES

static node * initNode(const size_t start, const size_t stop);

static void freeNode(node * n);

static head_node * initList(void);

static void freeList(head_node * head);

static void initEventRingBuffer(const size_t size_buffer);

static void freeEventRingBuffer(void);


//STATIC EVENT RING BUFFER MANIPULATION

static void _addNodeToList(head_node * head, node * n);

static void _popNodeFromList(head_node * head);

static size_t _getEventFromList(head_node * head, float * data);//malloc enough to data and fills it with the event

static _addEventToEventRingBuffer(const float * data, const size_t size_data);


//GLOBAL FUNCTIONS, called only once by main, don't need mutex

head_node * initEventDatastructure(const size_t size_buffer);

void freeEventDatastructure(head_node * head);


//MUTEX FUNCTIONS

void addNodeToList(head_node * head, node * n);

void popNodeFromList(head_node * head);

size_t getEventFromList(head_node * head, float * data);//malloc enough to data and fills it with the event

void addEventToEventRingBuffer(const float * data, const size_t size_data);

#endif

