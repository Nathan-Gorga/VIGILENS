#include "random_forest.h"

// Reads a CSV file with two columns: time, value
// Returns array of doubles with values, sets *out_len
double* read_csv_values(const char* filename, int* out_len) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("File open failed");
        *out_len = 0;
        return NULL;
    }

    char line[256];
    int capacity = 100;
    int count = 0;
    double* values = malloc(capacity * sizeof(double));
    if (!values) {
        perror("Memory allocation failed");
        fclose(fp);
        *out_len = 0;
        return NULL;
    }

    // Skip header line
    fgets(line, sizeof(line), fp);

    // Read each line
    double time, val;
    while (fscanf(fp, "%lf,%lf", &time, &val) == 2) {
        if (count >= capacity) {
            capacity *= 2;
            double* tmp = realloc(values, capacity * sizeof(double));
            if (!tmp) {
                perror("Realloc failed");
                free(values);
                fclose(fp);
                *out_len = 0;
                return NULL;
            }
            values = tmp;
        }
        values[count++] = val;
    }

    fclose(fp);
    *out_len = count;
    return values;
}



static int fArrMax(double signal[], const size_t size, double * ret){

    int idx = 0;
    double max = signal[0];

    for(int i = 1; i < size; i++){
        if(max < signal[i]){
            idx = i;
            max = signal[i];
        }
    }

    *ret = max;

    return idx;

}

static int fArrMin(double signal[], const size_t size, double * ret){

    int idx = 0;
    double min = signal[0];

    for(int i = 1; i < size; i++){
        if(min > signal[i]){
            idx = i;
            min = signal[i];
        }
    }

    *ret = min;

    return idx;

}


static inline double calculateSlope(const int x1, const double y1, const int x2, const double y2){

    return (y2 - y1) / (double)(x2 - x1);
}


static double estimateBaseline(double signal[], int length) {

    const double fraction = 0.1;

    if (length <= 0) return 0.0;

    int window = (int)(length * fraction);
    if (window < 1) window = 1;

    double sum = 0.0;
    int count = 0;

    // average first "window" samples
    for (int i = 0; i < window; i++) {
        sum += signal[i];
        count++;
    }

    // average last "window" samples
    for (int i = length - window; i < length; i++) {
        sum += signal[i];
        count++;
    }

    return sum / count;
}

static int findStart(double signal[], const size_t size, const double baseline, const double epsilon){
    //FIXME : find a way to automatically change depending on UART scale factor


    const double upper_boundary = baseline + epsilon;
    const double lower_boundary = fabs(baseline) >= epsilon ? baseline - epsilon : epsilon - baseline;

    int idx = 0;

    for(int i = idx; i < size; i++){

        if(lower_boundary <= signal[i] && signal[i] <= upper_boundary){

            idx++;

        }else break;
    }
    return idx;
}


static int findEnd(double signal[], const size_t size, const double baseline, const double epsilon){
    

    const double upper_boundary = baseline + epsilon;
    const double lower_boundary = fabs(baseline) >= epsilon ? baseline - epsilon : epsilon - baseline;

    int idx = size - 1;

    for(int i = idx; i >= 0; i--){

        if(lower_boundary <= signal[i] && signal[i] <= upper_boundary){

            idx--;
            
        }else break;
    }
    return idx;
}


static int findMiddle(const double* signal, int n, int idx_max, int idx_min, double baseline) {
    if (idx_max > idx_min) {
        int tmp = idx_max;
        idx_max = idx_min;
        idx_min = tmp;
    }

    int best_idx = idx_max;
    double best_dist = fabs(signal[idx_max] - baseline);

    for (int i = idx_max; i <= idx_min; i++) {
        double dist = fabs(signal[i] - baseline);
        if (dist < best_dist) {
            best_dist = dist;
            best_idx = i;
        }
    }

    return best_idx;
}


double compute_stddev(const double *signal, int length) {
 
    if (length <= 1) return 0.0;

    // Step 1: compute mean
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        sum += signal[i];
    }
    double mean = sum / length;

    // Step 2: compute variance
    double var = 0.0;
    for (int i = 0; i < length; i++) {
        double diff = signal[i] - mean;
        var += diff * diff;
    }
    var /= (length - 1);  // use N-1 for sample variance

    // Step 3: return standard deviation
    return sqrt(var);
}



double sample_entropy(const double *signal, int length, int m, double r) {

    if (length <= m + 1) return 0.0;

    int count_m = 0;
    int count_m1 = 0;

    // Loop over all template vectors
    for (int i = 0; i < length - m; i++) {
        for (int j = i + 1; j < length - m; j++) {
            int k;
            double dist = 0.0;

            // Check match of length m
            for (k = 0; k < m; k++) {
                double diff = fabs(signal[i + k] - signal[j + k]);
                if (diff > dist) dist = diff;  // Chebyshev distance
            }
            if (dist < r) {
                count_m++;

                // Check match of length m+1
                double diff = fabs(signal[i + m] - signal[j + m]);
                if (diff > dist) dist = diff;
                if (dist < r) {
                    count_m1++;
                }
            }
        }
    }

    if (count_m == 0 || count_m1 == 0) return INFINITY;  // no matches

    return -log((double)count_m1 / (double)count_m);
}

event_features featureExtraction(double event[], const size_t size){


    // calculate standard deviation
    const double std_dev = compute_stddev(event, size);

    const double epsilon = 0.05 * std_dev;

    //calculate baseline
    const double baseline = estimateBaseline(event, size);

    //find the index of start, the last baseline point before shifting from baseline
    const int start_idx = findStart(event, size, baseline, epsilon);

    //find the index of end, the first baseline point before remaining on baseline
    const int end_idx = findEnd(event, size, baseline, epsilon);

    // find highest peak
    double highest;
    const int highest_idx = fArrMax(event, size, &highest);

    //find lowest peak
    double lowest;
    const int lowest_idx = fArrMin(event, size, &lowest);

    //calculate slope 1 and slope 3
    const double slope_1 = calculateSlope(start_idx, baseline, highest_idx, highest);
    const double slope_3 = calculateSlope(lowest_idx, lowest, end_idx, baseline);

    // find the middle baseline point for slope 2

    //if the drop is too sharp, higheset and lowest may be separated by only one index, meaning slope 2 will be 0
    const int middle_idx = findMiddle(event,size, highest_idx, lowest_idx, baseline);

    //calculate slope 2
    const double slope_2 = calculateSlope(highest_idx, highest, middle_idx, baseline);

    //calculate t1, t2, and t3
    const int t1 = abs((int)middle_idx - (int)start_idx);
    const int t2 = abs((int)end_idx - (int)middle_idx);
    const int t3 =  abs((int)lowest_idx - (int)highest_idx);


    //calculate sample entropy
    const int m = 2;
    const double r = 0.2 * std_dev;

    const double samp_ent = sample_entropy(event, size, m, r);

    event_features ret = { slope_1, slope_2, slope_3, t1, t2, t3, std_dev, samp_ent };

    return ret;

}


void printFeatures(const event_features feat){
    printf("slope 1 : %f, slope 2 : %f, slope 3 : %f\n", feat.slope_1,feat.slope_2,feat.slope_3);
    printf("t1 : %zu, t2 : %zu, t3 : %zu\n", feat.t1,feat.t2,feat.t3);
    printf("standard deviation : %f, sample entropy: %f\n", feat.std_dev,feat.sample_entropy);
}


int main() {
    
    
    int len;
    double* vals = read_csv_values("./data/csv/blink3.csv", &len);

    if (!vals) return 1;
     
    event_features feat = featureExtraction(vals, len); 
     
    printFeatures(feat);
     
    free(vals);
    

    return 0;
}