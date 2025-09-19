#ifndef DATAINTAKE_H
#define DATAINTAKE_H

#include "../../globaldefinition.h"
#include "../../RINGBUFFER/ringbuffer.h"
#include "../../EVENTDATASTRUCTURES/eventdatastructure.h"
#include "../../ALGOS/THRESHOLD/threshold_algos.h"
#include "../../LOG/log.h"
#include "../../UART/uart.h"
#include "../../MOCKUART/mockuart.h"
#include "../../LED/led.h"

void * launchDataIntake(void * _);

extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;

extern int ready_count;


#endif
