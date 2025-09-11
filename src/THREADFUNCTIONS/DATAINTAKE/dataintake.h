#ifndef DATAINTAKE_H
#define DATAINTAKE_H

#include "../../globaldefinition.h"
#include "../../RINGBUFFER/ringbuffer.h"
#include "../../EVENTDATASTRUCTURES/eventdatastructure.h"
#include "../../ALGOS/algos.h"
#include "../../LOG/log.h"
#include "../../UART/uart.h"
#include "../../MOCKUART/mockuart.h"
#include "../../GRAPHING/graphing.h"




//#define PACKET_BUFFER_SIZE 10

#define TIME_IN_INTERNAL_RING_BUFFER 3//TODO change to fit the best openBCI stream settings
#define INTERNAL_RING_BUFFER_SIZE (int)(TIME_IN_INTERNAL_RING_BUFFER * SAMPLING_RATE * NUM_CHANNELS)

#define DATA_INTAKE_TEXT_COLOR BLUE
#define TIME_IN_WINDOW 1.2f
#define WINDOW_SIZE (size_t)(NUM_CHANNELS * SAMPLING_RATE * TIME_IN_WINDOW)

#define THRESH_MULT 4.0f



void * launchDataIntake(void * _);


extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;

extern int ready_count;


#endif
