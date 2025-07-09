#ifndef ALGOS__H
#define ALGOS__H  

#include "../globaldefinition.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

#define MIN_EVENT_DURATION 80 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)
#define MAX_EVENT_DURATION 300 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)

#define MIN_BASELINE_DURATION 30 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)

#define THRESHOLD_MAX 10 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)
#define THRESHOLD_MIN -10 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)

//this is because we detect the event only when it crosses baseline threshold, the event actually starts a few indexes before that
#define DETECTION_TOLERANCE 5 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)


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



size_t markEventsInBuffer(float buffer[], const size_t size_buffer, float events[][MAX_EVENT_DURATION], size_t size_events[]);



#endif







