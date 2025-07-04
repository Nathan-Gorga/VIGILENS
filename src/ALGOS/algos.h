#ifndef ALGOS__H
#define ALGOS__H  

#include "../globaldefinition.h"

//TODO : threshold detection both maximum and minimum
inline bool aboveThreshold(const float data_point, const float threshold);

inline bool belowThreshold(const float data_point, const float threshold);

//TODO : baseline detection (combien both max and minimum)
inline bool isBaseline(const float data_point, const float max, const float min);


#endif