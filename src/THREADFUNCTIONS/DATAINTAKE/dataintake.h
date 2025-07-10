#ifndef DATAINTAKE_H
#define DATAINTAKE_H

#include "../../globaldefinition.h"
#include "../../RINGBUFFER/ringbuffer.h"
#include "../../EVENTDATASTRUCTURES/eventdatastructure.h"
#include "../../ALGOS/algos.h"
#include "../../LOG/log.h"
#include "../../UART/uart.h"
#include "../../MOCKUART/mockuart.h"

#define PACKET_BUFFER_SIZE 10

#define TIME_IN_INTERNAL_RING_BUFFER 10
#define INTERNAL_RING_BUFFER_SIZE (size_t)(TIME_IN_INTERNAL_RING_BUFFER * SAMPLING_RATE * NUM_CHANNELS)

#define DATA_INTAKE_TEXT_COLOR BLUE


#ifdef DEBUG_INTAKE
    #define PRINTF_DEBUG do{printf(PURPLE"DEBUG : %s - %s:%d\n"RESET, __FILE__, __func__,__LINE__); fflush(stdout);}while(0);
#else
    #define PRINTF_DEBUG
#endif

void * launchDataIntake(void * arg);

static void dataIntake(void);

static void dataIntake(void);

static void cleanupHandler(void * internal_ring_buffer);

static void masterStartupDialogue(void);




extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;

extern int ready_count;


#endif
