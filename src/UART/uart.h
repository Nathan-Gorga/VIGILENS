/*
HEAVY INSPIRATION FROM : https://gist.github.com/ultramcu/cbdc598bbc1fcd0e5f8a

courtesy to https://github.com/ultramcu
*/

#ifndef UART_H
#define UART_H

#include "../globaldefinition.h"
#include "../EVENTDATASTRUCTURES/eventdatastructure.h"
#include "../ALGOS/FILTER/filter.h"

#include <fcntl.h>
#include <termios.h>


bool beginUART(void);

void endUART(void);

size_t getUARTData(double data_points[PACKET_BUFFER_SIZE]);

bool sendUARTSignal(const enum TX_SIGNAL_TYPE signal_type);

// global file descriptor of the UART socket
static int UART_fd;

#endif

