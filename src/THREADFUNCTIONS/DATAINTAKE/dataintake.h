#ifndef DATAINTAKE_H
#define DATAINTAKE_H

#include "../../globaldefinition.h"

#define DATA_INTAKE_TEXT_COLOR BLUE
#define printf(...) printf(DATA_INTAKE_TEXT_COLOR"DATA INTAKE : "); printf(__VA_ARGS__); printf(RESET)

void * launchDataIntake(void * arg);//TESTME : needs to be tested thoroughly to avoid segfaults

void dataIntake(void);




#endif
