#ifndef RANDOM_FOREST_H
#define RANDOM_FOREST_H

#include "../../globaldefinition.h"

typedef struct {

    double slope_1;
    double slope_2;
    double slope_3;
    
    int t1;
    int t2;
    int t3;
    
    double std_dev;

    double sample_entropy;

}event_features;


enum EVENT_TYPE{
    NOT_BLINK,
    BLINK,
    NUM_EVENT_TYPE
    // SLOW_BLINK,
    // FAST_BLINK,
    // SACCADE
};


#endif

