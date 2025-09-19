#ifndef EVENT_FEATURE_H
#define EVENT_FEATURE_H

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



event_features featureExtraction(double event[], const size_t size);


void printFeatures(const event_features feat);


#endif

