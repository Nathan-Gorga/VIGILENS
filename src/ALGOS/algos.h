#ifndef ALGOS__H
#define ALGOS__H  

#include "../globaldefinition.h"

//TODO : threshold detection both maximum and minimum
bool aboveThreshold(const float data_point, const float threshold);

bool belowThreshold(const float data_point, const float threshold);

//TODO : baseline detection (combien both max and minimum)
bool isBaseline(const float data_point, const float max, const float min);


#endif