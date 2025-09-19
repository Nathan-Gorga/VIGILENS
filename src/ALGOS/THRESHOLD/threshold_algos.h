#ifndef THRESHOLD_ALGOS_H
#define THRESHOLD_ALGOS_H

#include "../../globaldefinition.h"
#include "../../LED/led.h"


inline bool aboveThreshold(const double data_point, const double threshold);

inline bool belowThreshold(const double data_point, const double threshold);

inline bool isBaseline(const double data_point, const double max, const double min);

bool simpleThresholdEventDetection(const double threshold, const double event[MAX_EVENT_SIZE], const size_t size);

double compute_median(double *data, int length);

int adaptiveThreshold(
	double * eeg,
	const int signal_length,
	// const int sample_freq,
	// const double win_size,
	size_t * blink_indices,
	const double th_mult,
	bool * missing_data
);

#endif







