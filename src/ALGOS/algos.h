#ifndef ALGOS__H
#define ALGOS__H  

#include "../globaldefinition.h"

/**
 * @brief Check if a data point is above a certain threshold.
 *
 * @param data_point The data point to be compared.
 * @param threshold The threshold to compare against.
 *
 * @return true if the data point is above the threshold, false otherwise.
 */extern inline bool aboveThreshold(const float data_point, const float threshold);


/**
 * @brief Check if a data point is below or equal to a certain threshold.
 *
 * @param data_point The data point to be compared.
 * @param threshold The threshold to compare against.
 *
 * @return true if the data point is below or equal to the threshold, false otherwise.
 */extern inline bool belowThreshold(const float data_point, const float threshold);


/**
 * @brief Checks if a data point is within a given range.
 *
 * @param data_point The data point to be compared.
 * @param max The upper bound of the range.
 * @param min The lower bound of the range.
 *
 * @return true if the data point is within the range, false otherwise.
 */extern inline bool isBaseline(const float data_point, const float max, const float min);


#endif





