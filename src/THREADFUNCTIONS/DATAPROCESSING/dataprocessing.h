#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "../../globaldefinition.h"
#include "../../RINGBUFFER/ringbuffer.h"
#include "../../EVENTDATASTRUCTURES/eventdatastructure.h"

#define DATA_PROCESSING_TEXT_COLOR YELLOW
#define printf(...) printf(DATA_PROCESSING_TEXT_COLOR"DATA PROCESSING:%d - ",__LINE__); printf(__VA_ARGS__); printf(RESET)


void * launchDataProcessing(void * arg);

static void dataProcessing(void);

extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;

extern int ready_count;

#endif
