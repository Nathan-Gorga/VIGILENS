#include "threshold_algos.h"

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
	
	for(size_t i = 0; i < size; ++i){

		if(threshold <= event[i]) return true;

	}

	return false;
}

static int fArrMax(const double arr[], const int n){
	
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


static int find_local_maxima(double segment[], const int start, const int end){

	const int n = end - start;

	double window[MAX_EVENT_DURATION];

	memmove(window, segment + start, n * sizeof(double));

	return start + fArrMax(window, n);

}

static int cmp_double(const void *a, const void *b) {
    
	double da = *(double *)a;
    
	double db = *(double *)b;
    
	return (da > db) - (da < db);
}

// tried using gsl_stat function for median
// it didn't work properly, I may just be an idiot, 
// but my fix works and I don't care
// feel free to use gsl if it doesn't break
double compute_median(double *data, int length) {

    double *copy = malloc(length * sizeof(double));//FIXME : heap is slow, switch to arr[MAX_EVENT_DURATION] or larger

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

//TODO : implement stdThreshold alongside robust and test which is best
static double std_threshold(double * signal, const int start, const int end, const double th_mult){
	const int segment_length = end - start;

	if (segment_length <= 0) {

		fprintf(stderr, "Invalid segment length: %d\n", segment_length);

		return -1.0;

	}

    double *segment = (double *)malloc(segment_length * sizeof(double));//FIXME : heap is slow

	if (!segment) {

		perror("malloc failed");

		return -1.0;

	}

	//FIXME : switch this to memcpy
    for (int i = 0; i < segment_length; ++i) {

        segment[i] = signal[start + i];

	}

    const double med = compute_median(segment, segment_length);
	    
    free(segment);

	return med * th_mult;
}

static double robust_threshold(double * signal, const int start, const int end, const double th_mult){

	const int segment_length = end - start;

	if (segment_length <= 0) {

		fprintf(stderr, "Invalid segment length: %d\n", segment_length);

		return -1.0;

	}

    double *segment = (double *)malloc(segment_length * sizeof(double));//FIXME : heap is slow

	if (!segment) {

		perror("malloc failed");

		return -1.0;

	}

	//FIXME : switch this to memcpy
    for (int i = 0; i < segment_length; ++i) {

        segment[i] = signal[start + i];

	}

    const double med = compute_median(segment, segment_length);
	
    double *temp = (double *)malloc(segment_length * sizeof(double)); //FIXME : heap is slow

    if (!temp) {
    
		perror("malloc failed");
    
		free(segment);
    
		return -1.0;

    }

    for (int j = 0; j < segment_length; ++j) {

        temp[j] = fabs(signal[start + j] - med);  

    }

    const double mad = 1.4826 * compute_median(temp, segment_length);

    free(temp);
    free(segment);

	return med + th_mult * mad;

}

int adaptiveThreshold(
	float * eeg,
	const int signal_length,
	const int sample_freq,
	const float win_size,
	size_t * blink_indices,
	const float th_mult
) {
	
	const int win_len = (win_size * sample_freq);

	//TODO : implement a static refractory tracker to handle events that overlap on windows

	const int refractory_samples = REFRACTORY_SAMPLES;

	int count = 0;

	int prevent_overlap = 0;

	double signal[signal_length];

	for(int i = 0; i < signal_length; i++) signal[i] = fabs(eeg[i]);

	for(int i = 0; i < signal_length; i += win_len){

		const int end = min(i + win_len, signal_length - 1);

		const double threshold = std_threshold(signal, i,end, th_mult);//robust_threshold(signal, i , end, th_mult);

		printf("threshold : %f\n", threshold);

		for(int j = prevent_overlap; j < win_len; j++){

			const int idx = i + j;

			if(signal[idx] <= threshold) continue;

			const int limit = min(idx + refractory_samples, min(signal_length - 1, end + refractory_samples));

			blink_indices[count++] = find_local_maxima(signal, idx, limit);

			j += refractory_samples;

			prevent_overlap = max(j - win_len, 0);
		}
	}

	return count;
}
