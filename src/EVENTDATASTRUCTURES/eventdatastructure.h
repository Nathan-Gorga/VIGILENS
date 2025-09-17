#ifndef EVENTDATASTRUCTURE_H
#define EVENTDATASTRUCTURE_H

#include "../globaldefinition.h"
#include "../RINGBUFFER/ringbuffer.h"

#define TIME_IN_BUFFER 60

#define EVENT_RING_BUFFER_SIZE (size_t)(TIME_IN_BUFFER * SAMPLING_RATE * NUM_CHANNELS)

#define MAX_EVENT_SIZE (size_t)(NUM_CHANNELS * SAMPLING_RATE * 1/*seconds*/)

typedef struct node {
    struct node * next;
    size_t start;
    size_t stop;
}node;

typedef struct{
    node * next;
}head_node;

volatile static struct ring_buffer * event_ring_buffer;

volatile static head_node * head;

extern pthread_mutex_t event_ring_buffer_mutex;

extern pthread_mutex_t write_index_mutex; //this one is just to prevent double lock

extern pthread_mutex_t head_mutex;

extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;



void initEventDatastructure(const size_t size_buffer);

void freeEventDatastructure(void);

int createMutexes(void);

int destroyMutexes(void);

size_t getEvent(double * data);

void addEvent(double * data, const size_t size_data);

#endif

