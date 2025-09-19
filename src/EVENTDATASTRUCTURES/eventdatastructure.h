#ifndef EVENTDATASTRUCTURE_H
#define EVENTDATASTRUCTURE_H

#include "../globaldefinition.h"
#include "../RINGBUFFER/ringbuffer.h"

// node in the linked list that indexes events in the ERB
typedef struct node {
    struct node * next; // points to the next node in the list, NULL if it's the last
    size_t start; // the start index of the event in the ERB
    size_t stop; // the stop index of the event in the ERB
}node;

typedef struct{
    node * next; // for safety and ease of handling, I created a specific head node (only 1 instance should exist)
}head_node;

volatile static struct ring_buffer * event_ring_buffer;
extern pthread_mutex_t event_ring_buffer_mutex;

volatile static head_node * head;
extern pthread_mutex_t head_mutex;

extern pthread_mutex_t write_index_mutex; //this one is just to prevent double lock when writing to ERB

extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;


void initEventDatastructure(const size_t size_buffer);

void freeEventDatastructure(void);

int createMutexes(void);

int destroyMutexes(void);

size_t getEvent(double * data);

void addEvent(double * data, const size_t size_data);

#endif

