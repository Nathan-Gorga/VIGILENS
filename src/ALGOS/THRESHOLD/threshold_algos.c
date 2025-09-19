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

bool simpleThresholdEventDetection(const double threshold, const double event[MAX_EVENT_DURATION], const size_t size){
	
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


static void derivate(double signal[], const int size, double * output){
	// printf("derivating\n");
	for(int i = 0; i < size - 1; i++){

		output[i] = signal[i+1] - signal[i];
		// printf("%lf : %lf \n", signal[i], output[i]);
	}
}


// static double robust_threshold(double * signal, const int start, const int end, const double th_mult){

// 	const int segment_length = end - start;

// 	if (segment_length <= 0) {

// 		fprintf(stderr, "Invalid segment length: %d\n", segment_length);

// 		return -1.0;

// 	}

//     double *segment = (double *)malloc(segment_length * sizeof(double));//FIXME : heap is slow

// 	if (!segment) {

// 		perror("malloc failed");

// 		return -1.0;

// 	}

// 	//FIXME : switch this to memcpy
//     for (int i = 0; i < segment_length; ++i) {

//         segment[i] = signal[start + i];

// 	}

//     const double med = compute_median(segment, segment_length);
	
//     double *temp = (double *)malloc(segment_length * sizeof(double)); //FIXME : heap is slow

//     if (!temp) {
    
// 		perror("malloc failed");
    
// 		free(segment);
    
// 		return -1.0;

//     }

//     for (int j = 0; j < segment_length; ++j) {

//         temp[j] = fabs(signal[start + j] - med);  

//     }

//     const double mad = 1.4826 * compute_median(temp, segment_length);

//     free(temp);
//     free(segment);

// 	return med + th_mult * mad;

// }
//TODO : find a better fix
#define RIGHT_DOUBLE_BLINK_PREVENTION 70
#define LEFT_DOUBLE_BLINK_PREVENTION 90


int adaptiveThreshold(
	double * eeg,
	const int signal_length,
	// const int sample_freq,
	// const double win_size,
	size_t * blink_indices,
	const double th_mult,
	bool * missing_data
) {
// printf("in threshodl\n");
	*missing_data = false;
	
	// const int win_len = (win_size * sample_freq);

	//TODO : implement a static refractory tracker to handle events that overlap on windows

	const int refractory_samples = REFRACTORY_SAMPLES;

	int count = 0;

	// int prevent_overlap = 0;

	double signal[signal_length];

	for(int i = 0; i < signal_length; i++) signal[i] = fabs(eeg[i]);

	//calculate the threshold of the window
	const double threshold = std_threshold(signal, 0, signal_length, th_mult);

	int temp_blink[10];
	int temp_blink_size = 0;

	for(int i = 0; i < signal_length; i++){
		// printf("signal : %lf, threshold : %lf\n", signal[i], threshold);
		//detect all above the threshold
		if(eeg[i] < threshold) continue;

		// printf("passed threshodl\n");

		const int temp_leftB = max(0, i - refractory_samples);
		const int temp_rightB = min(signal_length - 1, i + refractory_samples);

		//find local maximum
		temp_blink[temp_blink_size++] = find_local_maxima(eeg,temp_leftB, temp_rightB);

		i += refractory_samples;
	}

	if(temp_blink_size == 0) return 0;

	//derivate signal
	double * derivated_signal = malloc((signal_length - 1) * sizeof(double));

	derivate(eeg, signal_length, derivated_signal);

	static bool db_blink = false;
	
	for(int i = 0; i < temp_blink_size; i++){

		int left_boundary = -1;
		int right_boundary = -1;
		
		int j;

		// printf("temp_blink : %d \n", temp_blink[i]);
		// printf("looking for left boundary\n");

		//find turning point in rising edge
		for(j = temp_blink[i]; derivated_signal[j-1] > 0 && j > 0; j--){
			// printf("j : %d, d : %lf\n",j, derivated_signal[j]);
			left_boundary = j;
		}
		// if(derivated_signal[j] < 0) left_boundary = j;
	

		// printf("looking for right boundary\n");

		//find turning point in falling edge
		for(j = temp_blink[i]; derivated_signal[j+1] < 0 && j < signal_length - 1; j++){
			// printf("j : %d, d : %lf\n",j+1, derivated_signal[j+1]);
			right_boundary = j;
		}
		// if(derivated_signal[j] > 0) right_boundary = j;
	
		// printf("left_boundary : %d, right_boundary : %d\n", left_boundary, right_boundary);

		if(right_boundary != -1 && right_boundary < signal_length - RIGHT_DOUBLE_BLINK_PREVENTION ){//&& left_boundary != -1 && left_boundary < signal_length - LEFT_DOUBLE_BLINK_PREVENTION){
			blink_indices[count++] = temp_blink[i];
			ledFlash();	
			db_blink = true;

		} 

		else if(left_boundary != -1){//} && left_boundary < signal_length - LEFT_DOUBLE_BLINK_PREVENTION){
			if(!db_blink) ledFlash();
			*missing_data = true;
			blink_indices[count++] = temp_blink[i];
			db_blink = false;
		}

		else{
			printf(RED"PREVENTED FALSE POSITIVE\n"RESET);
			db_blink = false;

		}

	}

		
	
	//if Peak, TP -> send the blink
	//if TP, Peak -> send the blink and flip a pointer flag to wait for the rest of the signal to arrive

	//if nothing -> send nothing-









	// for(int i = 0; i < signal_length ; i += win_len){

	// 	const int end = min(i + win_len, signal_length - 1);

	// 	const double threshold = std_threshold(signal, i,end, th_mult);//robust_threshold(signal, i , end, th_mult);

	// 	printf("threshold : %f\n", threshold);

	// 	for(int j = prevent_overlap; j < win_len; j++){

	// 		const int idx = i + j;

	// 		if(signal[idx] <= threshold) continue;

	// 		const int limit = min(idx + refractory_samples, min(signal_length - 1, end + refractory_samples));

	// 		blink_indices[count++] = find_local_maxima(signal, idx, limit);

	// 		j += refractory_samples;

	// 		prevent_overlap = max(j - win_len, 0);
	// 	}
	// }


	// const double threshold = std_threshold(signal, 0, signal_length, th_mult);
	// printf("threshold : %f\n", threshold);
	// for(int i = 0; i < signal_length; i++){
	// 	if(signal[i] < threshold) continue;
	// 	printf("i : %d, signal : %f\n", i, signal[i]);
	// 	blink_indices[count++] = find_local_maxima(signal, i, signal_length - 1);

	// 	i += refractory_samples; 
	// }

	free(derivated_signal);

	return count;
}
