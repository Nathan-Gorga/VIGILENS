#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "../../globaldefinition.h"

#define DATA_INTAKE_TEXT_COLOR YELLOW
#define printf(...) printf(DATA_INTAKE_TEXT_COLOR"DATA PROCESSING : "); printf(__VA_ARGS__); printf(RESET)


void * launchDataProcessing(void * arg);

static void dataProcessing(void);

#endif
