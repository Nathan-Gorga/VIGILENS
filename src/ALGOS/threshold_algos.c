#include "algos.h"

inline bool aboveThreshold(const float data_point, const float threshold){
	return data_point >= threshold;
}


inline bool belowThreshold(const float data_point, const float threshold){
	return data_point <= threshold;
}

inline bool isBaseline(const float data_point, const float max, const float min){

	const bool ret = !(aboveThreshold(data_point, max) || belowThreshold(data_point, min));

	return ret;
}



bool simpleThresholdEventDetection(const float threshold, const float event[MAX_EVENT_DURATION], const size_t size){
	for(int i = 0; i < size; ++i){

		if(threshold <= event[i]) return true;

	}

	return false;


}



int argmax(const double arr[], const int n){
	int max_index = 0;

	double max_value = arr[0];

	for(int i = 1; i < n; i++){
		if(arr[i] > max_value){

			max_value = arr[i];

			max_index = i;

		}
	}

	return max_index;

}



int find_local_maxima(double segment[], const int start, const int end){
	const int n = end - start;

	double * window = (double *)malloc(n * sizeof(double));

	memmove(window, segment + start, n * sizeof(double));

	const int ret = start + argmax(window, n);

	free(window);

	return ret;
}




static double robust_threshold(double * signal, const int start, const int end, const double th_mult){

	const int segment_length = end - start;

	double * segment = (double *)malloc(segment_length * sizeof(double));

	memmove(segment, signal + start, segment_length * sizeof(double));

	const double * test  = signal + start;

	const double med = gsl_stats_median(segment, 1, segment_length);

	double * temp = (double *)malloc(segment_length * sizeof(double));

	for(int j = 0; j < segment_length; j++){

		temp[j] = fabs(test[j] - med);

	}

	const double mad = 1.4826f * gsl_stats_median(temp, 1, segment_length);

	free(temp);

	free(segment);

	return med + th_mult * mad;

}



int adaptiveThreshold(double eeg[], const int signal_length, const int sample_freq, const float win_size, int * blink_indices, const float th_mult, double thresholds[]){

	const bool use_abs = true;

	double * signal = (double *)malloc(signal_length * sizeof(double));

	const int n_samples = signal_length;

	const int win_len = (int)(win_size * sample_freq);

	const int refractory_samples = (REFRACTORY_PERIOD * sample_freq);

	int thresh_count= 0;

	int count = 0;

	int prevent_window_overflow = 0;

	for(int i = 0; i < signal_length; ++i){
		signal[i] = use_abs ? fabs(eeg[i]) : eeg[i];
	}

	for(int i = 0; i < n_samples; i+=win_len){
		const int end_of_window = fmin(i + win_len, n_samples-1);

		const double threshold = robust_threshold(signal, i , end_of_window, th_mult);

		thresholds[thresh_count++] = threshold;

		for(int j = prevent_window_overflow; j < win_len; j++){

			if(signal[i + j] > threshold){

				int limit = fmin(j + i + refractory_samples, end_of_window);

				if(limit == end_of_window && limit != n_samples - 1){//too complicated, make it simpler
					limit += refractory_samples;
				}

				blink_indices[count++] = find_local_maxima(signal, j + i, limit);

				j += refractory_samples;
			}

			prevent_window_overflow = fmax(j - win_len, 0);
		}
	}

	free(signal);

	return count;

}
