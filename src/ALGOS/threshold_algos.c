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

int cmp_double(const void *a, const void *b) {
    double da = *(double *)a;
    double db = *(double *)b;
    return (da > db) - (da < db);
}

double compute_median(double *data, int length) {
    double *copy = malloc(length * sizeof(double));
    memcpy(copy, data, length * sizeof(double));
    qsort(copy, length, sizeof(double), cmp_double);

    double median;
    if (length % 2 == 0) {
        median = 0.5 * (copy[length / 2 - 1] + copy[length / 2]);
    } else {
        median = copy[length / 2];
    }

    free(copy);
    return median;
}

#define MIN_THRESHOLD_BOUNDARY 0.0
//FIXME : threshold is negative here
static double robust_threshold(double * signal, const int start, const int end, const double th_mult){

	// for(int i = 0; i < 10; i++){
	// 	printf("%d : %f\n", i, signal[i]);
	// }

	const int segment_length = end - start;
    if (segment_length <= 0) {
        fprintf(stderr, "Invalid segment length: %d\n", segment_length);
        return -1.0;
    }

    double *segment = (double *)malloc(segment_length * sizeof(double));
    if (!segment) {
        perror("malloc failed");
        return -1.0;
    }

    for (int i = 0; i < segment_length; ++i) {
        segment[i] = signal[start + i];
		// printf("%d : %f\n", i, segment[i]);
    }

    // Sort before median
    
    const double med = compute_median(segment, segment_length);
	// printf("med : %f\n", med);

    // for (int i = 0; i < 10; ++i) {
    //     // segment[i] = signal[start + i];
	// 	printf("%d : %f\n", i, segment[i]);
    // }

	
    double *temp = (double *)malloc(segment_length * sizeof(double));
    if (!temp) {
        perror("malloc failed");
        free(segment);
        return -1.0;
    }

    for (int j = 0; j < segment_length; ++j) {
        temp[j] = fabs(signal[start + j] - med);  // Use fabs twice to be safe
    }

    const double mad = 1.4826 * compute_median(temp, segment_length);

	// printf("mad : %f\n", mad);

    free(temp);
    free(segment);

    const double ret = med + th_mult * mad;

    // printf("threshold: %f\n", ret);

    return ret;

}



int adaptiveThreshold(
	float * eeg,
	const int signal_length,
	const int sample_freq,
	const float win_size,
	int * blink_indices,
	const float th_mult,
	double * threshold
) {
	// for(int i = 0; i < 10; i++){
	// 	printf("%d : %f\n", i , eeg[i]);
	// }

	const int win_len = (win_size * sample_freq);

	const int refractory_samples = (REFRACTORY_PERIOD * sample_freq);

	int count = 0;

	int prevent_overlap = 0;

	double signal[signal_length];


	for(int i = 0; i < signal_length; i++) signal[i] = fabs(eeg[i]);

	// for(int i = 0; i < 10; i++){
	// 	printf("%d : %f\n", i , signal[i]);
	// }


	for(int i = 0; i < signal_length; i += win_len){

		const int end = min(i + win_len, signal_length - 1);

		*threshold = robust_threshold(signal, i , end, th_mult);

		printf("THRESHOLD : %f\n", *threshold);

		for(int j = prevent_overlap; j < win_len; j++){

			const int idx = i + j;

			if(signal[idx] <= *threshold) continue;

			const int limit = min(idx + refractory_samples, min(signal_length - 1, end + refractory_samples));

			blink_indices[count++] = find_local_maxima(signal, idx, limit);

			j += refractory_samples;

			prevent_overlap = max(j - win_len, 0);
		}
	}

	return count;
}
