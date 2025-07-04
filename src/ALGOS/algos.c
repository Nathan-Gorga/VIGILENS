#include "algos.h"

inline bool aboveThreshold(const float data_point, const float threshold){
    return data_point > threshold;
}

inline bool belowThreshold(const float data_point, const float threshold){
    return data_point < threshold;
}

inline bool isBaseline(const float data_point, const float max, const float min){
    /*
    NOT ( TRUE OR TRUE) = FALSE 
    NOT (TRUE OR FALSE) = FALSE
    NOT (FALSE OR TRUE) = FALSE
    NOT (FALSE OR FALSE) = TRUE
    */
    return !(aboveThreshold(data_point, max) || belowThreshold(data_point, min));
}


