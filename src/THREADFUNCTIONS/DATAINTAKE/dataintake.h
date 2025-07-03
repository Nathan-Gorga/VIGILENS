#ifndef DATAINTAKE_H
#define DATAINTAKE_H

#include "../../globaldefinition.h"
#include "../../RINGBUFFER/ringbuffer.h"
#include "../../EVENTDATASTRUCTURES/eventdatastructure.h"

#define TIME_IN_INTERNAL_RING_BUFFER 10
#define INTERNAL_RING_BUFFER_SIZE (size_t)(TIME_IN_INTERNAL_RING_BUFFER * SAMPLING_RATE * NUM_CHANNELS)

#define DATA_INTAKE_TEXT_COLOR BLUE
#define printf(...) printf(DATA_INTAKE_TEXT_COLOR"DATA INTAKE : "); printf(__VA_ARGS__); printf(RESET)

void * launchDataIntake(void * arg);

static void dataIntake(void);




#endif
