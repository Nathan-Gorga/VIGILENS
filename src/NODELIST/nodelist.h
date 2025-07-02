#ifndef NODELIST_H
#define NODELIST_H

#include "../globaldefinition.h"


static struct ring_buffer event_ring_buffer;

//TODO : node struct 
typedef struct node{
    struct node * next;
    size_t start;
    size_t stop;
}node;

//TODO : init node 

//TODO : free node 


//TODO : init list () which inits the ring buffer and the head

//TODO : add node to list (static)

//TODO : pop node from list (static)

//TODO : get most recent event from list (static)

//TODO : build mutex functions that are not static to prevent race conditions









#endif

