#ifndef NODELIST_H
#define NODELIST_H

#include "../globaldefinition.h"
#include "../RINGBUFFER/ringbuffer.h"


typedef struct node{
    struct node * next;
    size_t start;
    size_t stop;
}node;

typedef struct head_node{
    struct node * next;
}head_node;

static struct ring_buffer * event_ring_buffer;

static head_node * head;

//STATIC INITS AND FREES

static node * initNode(const size_t start, const size_t stop);

static void freeNode(node * n);

static void initList(void);

static void freeList(void);

static void initEventRingBuffer(const size_t size_buffer);

static void freeEventRingBuffer(void);


//STATIC EVENT RING BUFFER MANIPULATION

static void _addNodeToList(node * n);

static void _popNodeFromList(void);

static size_t _getEvent(float * data);//malloc enough to data and fills it with the event

static void _addEvent(const float * data, const size_t size_data);

static inline size_t _getWriteIndex(void);

size_t getWriteIndex(void);

//GLOBAL FUNCTIONS, called only once by main, don't need mutex

void initEventDatastructure(const size_t size_buffer);

void freeEventDatastructure(void);


//MUTEX FUNCTIONS

void addNodeToList(node * n);

void popNodeFromList(void);

size_t getEvent(float * data);//malloc enough to data and fills it with the event

void addEvent(const float * data, const size_t size_data);

#endif

