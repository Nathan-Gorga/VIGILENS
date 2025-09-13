#include "random_forest.h"

int cmpfunc(const void * a, const void * b){
    const double fa = ((sample *)a)->value;
    const double fb = ((sample *)b)->value;
    
    if(fa < fb) return -1;
    else if (fa > fb) return 1;
    else return 0;

}

double giniImpurity(int * class_counts, const int total, const int n_classes){
    if(total == 0) return 0.0f;
    double gini = 1.0f;
    for(int i = 0; i < n_classes; i++){
        double p = (double)class_counts[i]/total;
        gini -= p * p;
    }

    return gini;

}


double bestThreshold(double *x, int *y, const int n_samples, const int n_classes, double * best_gini){
    sample * samples = malloc(n_samples * sizeof(sample));
    for(int i = 0; i < n_samples; i++){
        samples[i].value = x[i];
        samples[i].label = y[i];
    }

    qsort(samples, n_samples, sizeof(sample), cmpfunc);

    int * left_counts = calloc(n_classes, sizeof(int));
    int * right_counts = calloc(n_classes, sizeof(int));

    for(int i = 0; i < n_samples; i++){

        right_counts[samples[i].label]++;
    }

    int left_total = 0;
    int right_total = n_samples;

    double best_tresh = samples[0].value;
    *best_gini = FLT_MAX;

    for(int i = 0; i < n_samples - 1; i++){
        
        
        int cls = samples[i].label;
        left_counts[cls]++;
        right_counts[cls]--;
        left_total++;
        right_total--;


        if(samples[i].label != samples[i + 1].label){
            double threshold = (samples[i].value + samples[i+1].value) / 2.0f;

            double gini_left = giniImpurity(left_counts, left_total, n_classes);
            double gini_right = giniImpurity(right_counts, right_total, n_classes);
            double weighted_gini = (left_total * gini_left + right_total * gini_right) / n_samples;

            if(weighted_gini < *best_gini){
                *best_gini = weighted_gini;
                best_tresh = threshold;
            }

        }

    }

    free(samples);
    free(left_counts);
    free(right_counts);
    return best_tresh;

}


int arrmin(double arr[], const int size){
    double min = arr[0];
    int idx = 0;

    for(int i = 1; i < size; i++){
        if(arr[i] < min){
            min = arr[i];
            idx = i;
        }
    }

    return idx;
}

int arrmax(double arr[], const int size){
    double max = arr[0];

    int idx = 0; 

    for(int i = 1; i < size; i++){
        if(arr[i] > max){
            max = arr[i];
            idx = i;
        }
    }

    return idx;
}

int iarrmax(int arr[], const int size){
    int max = arr[0];

    int idx = 0; 

    for(int i = 1; i < size; i++){
        if(arr[i] > max){
            max = arr[i];
            idx = i;
        }
    }

    return idx;
}



tree_node * createNode(const enum NODE_TYPE type, const int feature_index, const double threshold, const enum EVENT_TYPE class_label){

    tree_node * node = (tree_node *)malloc(sizeof(tree_node)); 

    if(node == NULL) return NULL;

    node->type = type;

    node->left = NULL;
    node->right = NULL;

    node->feature_index = feature_index;

    node->threshold = threshold;

    node->label = class_label;

    return node;
}


void printNode(tree_node * n){
    printf("type : %d\n", n->type);
    printf("left : %p\n", n->left);
    printf("right : %p\n", n->right);
    printf("feature idx : %d\n", n->feature_index);
    printf("threshold : %f\n", n->threshold);
    printf("label : %d\n", n->label);
}

#define MIN_SAMPLES 3
#define MAX_DEPTH 10


bool sameClass(double ** data, const size_t rows, const size_t cols){

    // printf("rows : %d, cols : %d\n", rows, cols ); fflush(stdout);
    for(int i = 0; i < rows - 1; i++){
        
        const double label1 = data[i][cols-1];
        
        const double label2 = data[i + 1][cols-1];
        // printf("%f != %f\n", label1, label2);
        if(label1 != label2) return false;
    }

    return true;
}


enum EVENT_TYPE majorityClass(double ** data,const size_t rows,const size_t cols){
    
    int count[NUM_EVENT_TYPE] = {0};

    // printf("In this dataset, there are %zu rows\n", rows); fflush(stdout);

    for(int i = 0; i < rows; i++){
    
        const int idx = data[i][cols-1];
        // printf("%d\n", idx);
        count[idx]++;
    
    }

    return iarrmax(count, NUM_EVENT_TYPE);

}



tree_node * buildTree(double ** data, const size_t rows, const size_t cols, const size_t depth){


    // printf("In depth %zu\n", depth);

    //1. check stopping conditions
    if(rows <= MIN_SAMPLES || depth >= MAX_DEPTH || sameClass(data, rows, cols)){

        // printf("creating a leaf node\n");
        const enum EVENT_TYPE majority = majorityClass(data, rows, cols);


        return createNode(LEAF, -1, -1, majority);
    }

    //2. find the best split (feature + threshold)
    const int n_samples = rows, n_classes = NUM_EVENT_TYPE;

    int  * label_data = malloc(n_samples * sizeof(int));
    for(int i = 0; i < n_samples; i++){
        label_data[i] = data[i][cols - 1];
    }

    double * feature_data = malloc(n_samples * sizeof(double));

    double * gini = malloc((cols-1) * sizeof(double));

    double * threshold = malloc((cols-1) * sizeof(double));

    for(int i = 0; i < cols -1; i++){

        for(int j = 0; j < n_samples; j++){
            
            feature_data[j] = data[j][i];

        }


        threshold[i] = bestThreshold(feature_data, label_data, n_samples, n_classes, &gini[i]);
    
        // printf("for feature %d, threshold is %f with gini : %f\n", i, threshold[i], gini[i]);
    
    }
    
    const int best_feature = arrmin(gini, cols -1);

    const double best_threshold = threshold[best_feature];
    
    free(gini);
    
    free(threshold);

    free(feature_data);
    
    free(label_data);
    
    
    //3. partition dataset
    double ** left_data = malloc(n_samples * sizeof(double *));
    double ** right_data = malloc(n_samples * sizeof(double *));

    int left_rows = 0;
    int right_rows = 0;

    for(int i = 0; i < n_samples; i++){

        if(data[i][best_feature] >= best_threshold){

            left_data[left_rows] = malloc(cols * sizeof(double));

            memcpy(left_data[left_rows], data[i], cols * sizeof(double));

            left_rows++;
        }else{

            right_data[right_rows] = malloc(cols * sizeof(double));

            memcpy(right_data[right_rows], data[i], cols * sizeof(double));

            right_rows++;
        }
    }

    //4. create node
    enum NODE_TYPE node_type = depth != 0 ? BRANCH : ROOT;

    tree_node * node = createNode(node_type, best_feature, best_threshold, OTHER);

    //5. recurse on children
    // printf("Going into left branch\n");
    node->left = buildTree(left_data, left_rows, cols, depth + 1);

    // printf("Going into right branch\n");
    node->right = buildTree(right_data, right_rows, cols, depth + 1);


    for(int i = 0; i < left_rows; i++){
        free(left_data[i]);
    }
    free(left_data);

    for(int i = 0; i < right_rows; i++){
        free(right_data[i]);
    }
    free(right_data);

    // printf("finished with depth %zu\n", depth);

    return node;

}

enum EVENT_TYPE predict(const tree_node *node, const double *sample) {
    if (node == NULL) {
        // safety fallback
        return OTHER;
    }

    if (node->type == LEAF) {
        return node->label;
    }

    if (sample[node->feature_index] >= node->threshold) {
        return predict(node->left, sample);
    } else {
        return predict(node->right, sample);
    }
}

enum EVENT_TYPE predictForest(random_forest * f, const double * sample){

    if(f == NULL) return OTHER;


    int vote[NUM_EVENT_TYPE] = {0};

    for(int i = 0; i < f->size; i++){

        // printf("Querying tree %d\n",i+1);

        const int idx = predict(f->forest[i], sample);

        if(idx != -1) vote[idx]++;

    }  

    return iarrmax(vote, NUM_EVENT_TYPE);

}



void freeTree(tree_node * n){

    if(n == NULL) return;

    freeTree(n->left);
    freeTree(n->right);

    free(n);
}

void printTree(const tree_node *node, int depth) {
    if (node == NULL) {
        return;
    }

    // indent according to depth
    for (int i = 0; i < depth; i++) {
        printf("    "); // 4 spaces per depth
    }

    if (node->type == LEAF) {
        printf("Leaf: class = %d\n", node->label);
    } else {
        printf("Node: feature[%d] <= %.3f ?\n", node->feature_index, node->threshold);
    }

    // recurse left/right
    if (node->left) {
        for (int i = 0; i < depth; i++) printf("    ");
        printf("Left:\n");
        printTree(node->left, depth + 1);
    }

    if (node->right) {
        for (int i = 0; i < depth; i++) printf("    ");
        printf("Right:\n");
        printTree(node->right, depth + 1);
    }
}


double ** bagging(double **data, const size_t rows, const size_t cols, const size_t iterations){

    double ** agg_data = malloc(iterations * sizeof(double*));

    for(int i = 0; i < iterations; i++){

        const int random_row = rand() % rows;


        agg_data[i] = malloc(cols * sizeof(double));

        memcpy(agg_data[i], data[random_row], cols * sizeof(double));

    }

    return agg_data;

}


random_forest * createForest(const size_t size, const size_t max_depth){

    random_forest * forest = malloc(sizeof(random_forest));

    if(!forest) return NULL;

    forest->size = size;
    forest->max_depth = max_depth;

    forest->forest = malloc(size * sizeof(tree_node*));

    return forest;
}

void freeForest(random_forest * f){
    
    for(int i = 0; i < f->size; i++){

        freeTree(f->forest[i]);

    }

    free(f->forest);
    free(f);
}



random_forest * buildForest(
    double ** data, 
    const size_t rows, 
    const size_t cols, 
    const size_t forest_size,
    const size_t forest_depth,
    const size_t bagging_size
){

    random_forest * f = createForest(forest_size, forest_depth);

    if(!f) return NULL;

    for(int i = 0; i < forest_size; i++){

        double ** tree_data = bagging(data, rows, cols, bagging_size);

        f->forest[i] = buildTree(tree_data, bagging_size, cols, 0);

        for(int j = 0; j < bagging_size; j++){
    
            free(tree_data[j]);

        }

        free(tree_data);
    }

    return f;

}















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
    printf("t1 : %d, t2 : %d, t3 : %d\n", feat.t1,feat.t2,feat.t3);
    printf("standard deviation : %f, sample entropy: %f\n", feat.std_dev,feat.sample_entropy);
}



// Write header once
void write_csv_header(FILE *fp) {
    fprintf(fp,
        "slope1,slope2,slope3,"
        "t1,t2,t3,"
        "std_dev,sample_entropy,label\n"
    );
}

void process_folder(const char *foldername, const char *outfile) {
    DIR *dir;
    struct dirent *entry;
    FILE *fp_out = fopen(outfile, "w");
    if (!fp_out) {
        perror("Failed to open output CSV");
        return;
    }

    write_csv_header(fp_out);

    dir = opendir(foldername);
    if (!dir) {
        perror("Failed to open folder");
        fclose(fp_out);
        return;
    }

    char filepath[1024];
    while ((entry = readdir(dir)) != NULL) {
        // skip . and ..
        if (entry->d_name[0] == '.') continue;

        // only process .csv files
        const char *ext = strrchr(entry->d_name, '.');
        if (!ext || strcmp(ext, ".csv") != 0) continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", foldername, entry->d_name);

        int len;
        double *vals = read_csv_values(filepath, &len);
        if (!vals || len == 0) {
            fprintf(stderr, "Skipping %s (empty or error)\n", filepath);
            free(vals);
            continue;
        }

        event_features feat = featureExtraction(vals, len);

        int label = BLINK;
        if(strstr(entry->d_name, "not_blink")){
            label = NOT_BLINK;
        }

        // write row
        fprintf(fp_out, "%f,%f,%f,%d,%d,%d,%f,%f,%d\n",
            feat.slope_1, feat.slope_2, feat.slope_3,
            feat.t1, feat.t2, feat.t3,
            feat.std_dev, feat.sample_entropy,
            label
        );

        free(vals);
    }

    closedir(dir);
    fclose(fp_out);
}

int main() {

    srand(time(NULL));

    size_t rows, cols;
    
    //extract data from CSV file
    double ** data = getNumericData("eeg_blinks.csv", &rows, &cols);

    const int training_rows = rows;
    const int eval_rows = rows - 50;

    const int bagging_size = rows;
    const int forest_size = 10;

    random_forest * forest = buildForest(data, training_rows, cols, forest_size, MAX_DEPTH, bagging_size);

    int vote = 0;

    for(int i = 50; i < rows; i++){

        const double * sample = data[i];

        // for(int j = 0; j < cols; j++){
        //     printf("%f, ", sample[j]);
        // }printf("\n");
        // printf("%f\n", sample[cols - 1]);


        const enum EVENT_TYPE prediction = predictForest(forest, sample);

        // printf("%d\n", sample[cols-1]);

        printf("forest says : %d, answer is %d\n", prediction, (int)sample[cols - 1]);

        if((int)prediction == (int)sample[cols-1]) vote++;

    }

    printf("PREDICTION SCORE : %.1f% (%d/%d)\n",((float)vote/(float)eval_rows) * 100, vote, eval_rows);

    freeForest(forest);

    for(int i = 0; i < rows; i++){
    
        free(data[i]);

    }

    free(data);

    return 0;
}


