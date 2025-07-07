#ifndef MAIN_H
#define MAIN_H

#include "globaldefinition.h"
#include "THREADFUNCTIONS/DATAINTAKE/dataintake.h"
#include "THREADFUNCTIONS/DATAPROCESSING/dataprocessing.h"
#include "RINGBUFFER/ringbuffer.h"
#include "EVENTDATASTRUCTURES/eventdatastructure.h"
#include "UART/uart.h"

#define MAIN_TEXT_COLOR GREEN
#define printf(...) printf(MAIN_TEXT_COLOR"MASTER:%d - ",__LINE__); printf(__VA_ARGS__); printf(RESET)

//TODO : write better documentation


#endif
