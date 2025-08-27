#ifndef ALGOS__H
#define ALGOS__H

#include "../globaldefinition.h"
#include "../THREADFUNCTIONS/DATAINTAKE/dataintake.h"

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

#define MIN_EVENT_DURATION 80 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)
#define MAX_EVENT_DURATION 300 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)

#define MIN_BASELINE_DURATION 30 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)

#define THRESHOLD_MAX 5.0f //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)
#define THRESHOLD_MIN -5.0f //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)

//this is because we detect the event only when it crosses baseline threshold, the event actually starts a few indexes before that
#define DETECTION_TOLERANCE 5 //TRY : different values for best results (will need experimentation as well as maybe a calibration phase to get this down)
#define REFRACTORY_PERIOD 0.35f


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


/**
 * @brief Identifies and marks events in a data buffer based on baseline shifts.
 *
 * @param buffer The input buffer containing data points to be analyzed.
 * @param size_buffer The number of data points in the buffer.
 * @param events A 2D array to store detected events, where each event is a sequence of data points.
 * @param size_events An array to store the sizes of the detected events.
 *
 * @return The number of events detected and marked in the buffer.
 *
 * @details This function analyzes a buffer of data points to identify segments that represent events.
 *          An event is detected when a segment of the data deviates from a baseline, which is defined
 *          using thresholds. The function first determines whether each data point is part of the baseline
 *          or a signal. It then identifies contiguous segments of signals and checks if they meet the
 *          minimum and maximum duration criteria. Events that meet these criteria are copied into the
 *          events array, and their sizes are recorded in size_events. The function returns the total
 *          number of events detected. The function ensures that memory is properly managed and checks
 *          for null pointers and invalid sizes.
 */size_t markEventsInBuffer(float buffer[], const size_t size_buffer, float events[][MAX_EVENT_DURATION], size_t size_events[]);



bool simpleThresholdEventDetection(const float threshold, const float event[MAX_EVENT_DURATION], const size_t size);


typedef struct {
	float b0, b1, b2;
	float a1, a2;
	float z1, z2;
} iir_filter;



void iirFilterInit(iir_filter * filt, const float b0, const float b1, const float b2, const float a1, const float a2);

float iirFilterProcess(iir_filter * filt, const float x);

void setupFilter(void);

float filterDataPoint(const float point);

int argmax(const double arr[], const int n);

int find_local_maxima(double segment[], const int start, const int end);

int adaptiveThreshold(double eeg[], const int signal_length, const int sample_freq, const float win_size, int * blink_indices, const float th_mult, double thresholds[]);


#endif







