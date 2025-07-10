#ifndef MAIN_H
#define MAIN_H

#include "globaldefinition.h"
#include "THREADFUNCTIONS/DATAINTAKE/dataintake.h"
#include "THREADFUNCTIONS/DATAPROCESSING/dataprocessing.h"
#include "RINGBUFFER/ringbuffer.h"
#include "EVENTDATASTRUCTURES/eventdatastructure.h"
#include "LOG/log.h"
#include "UART/uart.h"

#define MAIN_TEXT_COLOR GREEN

//TODO : write better documentation


#ifdef DEBUG_MAIN
    #define PRINTF_DEBUG do{printf(PURPLE"DEBUG : %s - %s:%d\n"RESET, __FILE__, __func__,__LINE__); fflush(stdout);}while(0);
#else
    #define PRINTF_DEBUG
#endif

#endif
