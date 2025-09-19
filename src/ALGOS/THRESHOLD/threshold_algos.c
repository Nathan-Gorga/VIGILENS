#include "threshold_algos.h"

inline bool aboveThreshold(const double data_point, const double threshold){
	return data_point >= threshold;
}


inline bool belowThreshold(const double data_point, const double threshold){
	return data_point <= threshold;
}

inline bool isBaseline(const double data_point, const double max, const double min){

	const bool ret = !(aboveThreshold(data_point, max) || belowThreshold(data_point, min));

	return ret;
}

bool simpleThresholdEventDetection(const double threshold, const double event[MAX_EVENT_SIZE], const size_t size){
	
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

	double window[MAX_EVENT_SIZE];

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

	memmove(segment, signal + start, segment_length * sizeof(double));

    const double med = compute_median(segment, segment_length);
	    
    free(segment);

	return med * th_mult;
}


static void derivate(double signal[], const int size, double * output){

	for(int i = 0; i < size - 1; i++){

		output[i] = signal[i+1] - signal[i];

	}
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

	memmove(segment, signal + start, segment_length * sizeof(double));

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
	double * eeg,
	const int signal_length,
	size_t * blink_indices,
	const double th_mult,
	bool * missing_data
) {

	*missing_data = false;

	const int refractory_samples = REFRACTORY_SAMPLES;

	int count = 0;

	double signal[signal_length];

	for(int i = 0; i < signal_length; i++) signal[i] = fabs(eeg[i]);

	const double threshold = std_threshold(signal, 0, signal_length, th_mult);

	int temp_blink[10];
	int temp_blink_size = 0;

	for(int i = 0; i < signal_length; i++){

		//detect all above the threshold
		if(eeg[i] < threshold) continue;

		const int temp_leftB = max(0, i - refractory_samples);
		const int temp_rightB = min(signal_length - 1, i + refractory_samples);

		//find local maximum
		temp_blink[temp_blink_size++] = find_local_maxima(eeg,temp_leftB, temp_rightB);

		i += refractory_samples;
	}

	if(temp_blink_size == 0) return 0;

	//derivate signal
	double * derivated_signal = malloc((signal_length - 1) * sizeof(double));//FIXME : HEAP IS SLOW

	derivate(eeg, signal_length, derivated_signal);

	static bool db_blink = false;
	
	for(int i = 0; i < temp_blink_size; i++){

		int left_boundary = -1;
		int right_boundary = -1;
		
		int j;

		//find turning point in rising edge
		for(j = temp_blink[i]; derivated_signal[j-1] > 0 && j > 0; j--){
			left_boundary = j;
		}

		//find turning point in falling edge
		for(j = temp_blink[i]; derivated_signal[j+1] < 0 && j < signal_length - 1; j++){
			right_boundary = j;
		}

		if(right_boundary != -1 && right_boundary < signal_length - 70 ){
			blink_indices[count++] = temp_blink[i];
			ledFlash();	
			db_blink = true;

		} 

		else if(left_boundary != -1 && left_boundary < signal_length - 90){
			if(!db_blink) ledFlash();
			*missing_data = true;
			blink_indices[count++] = temp_blink[i];
			db_blink = false;
		}


	}

	free(derivated_signal);

	return count;
}
