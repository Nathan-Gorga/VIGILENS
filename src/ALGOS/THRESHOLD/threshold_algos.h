#ifndef THRESHOLD_ALGOS_H
#define THRESHOLD_ALGOS_H

#include "../../globaldefinition.h"

#define MAX_EVENT_DURATION (size_t)(SAMPLING_RATE * NUM_CHANNELS * 1.0f /*seconds*/)

#define REFRACTORY_SAMPLES (size_t)(SAMPLING_RATE * REFRACTORY_PERIOD)

#define BLINK_DURATION 0.4f//in seconds

#define REFRACTORY_PERIOD 0.3f //seconds

#define NUM_SAMPLES_IN_BLINK (size_t)(BLINK_DURATION * SAMPLING_RATE * NUM_CHANNELS)

// best is 9.0f
#define THRESH_MULT 9.0f


inline bool aboveThreshold(const float data_point, const float threshold);

inline bool belowThreshold(const float data_point, const float threshold);

inline bool isBaseline(const float data_point, const float max, const float min);

bool simpleThresholdEventDetection(const float threshold, const float event[MAX_EVENT_DURATION], const size_t size);

double compute_median(double *data, int length);

int adaptiveThreshold(
	float * eeg,
	const int signal_length,
	const int sample_freq,
	const float win_size,
	size_t * blink_indices,
	const float th_mult
);

#endif







