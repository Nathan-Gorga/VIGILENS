#include "build_forest.h"




static int cmpfunc(const void * a, const void * b){
    const double fa = ((sample *)a)->value;
    const double fb = ((sample *)b)->value;
    
    if(fa < fb) return -1;
    else if (fa > fb) return 1;
    else return 0;

}

static double giniImpurity(int * class_counts, const int total, const int n_classes){
    if(total == 0) return 0.0f;
    double gini = 1.0f;
    for(int i = 0; i < n_classes; i++){
        double p = (double)class_counts[i]/total;
        gini -= p * p;
    }

    return gini;

}


static double bestThreshold(double *x, int *y, const int n_samples, const int n_classes, double * best_gini){
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


static int arrmin(double arr[], const int size){
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

static int arrmax(double arr[], const int size){
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

static int iarrmax(int arr[], const int size){
    int max = arr[0];

    int idx = 0; 

    for(int i = 1; i < size; i++){
        if(arr[i] >= max){
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

    tree_node * node = createNode(node_type, best_feature, best_threshold, NOT_BLINK/*OTHER*/);

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




int main(void) {

    srand(time(NULL));

    size_t rows, cols;
    
    //extract data from CSV file
    double ** data = getNumericData("new_eeg_data.csv", &rows, &cols);

    const double data_split = 0.2;


    printf("total rows of data : %d\n", rows);
    const int training_rows = (rows ) - (int)(rows * data_split);

    printf("training rows : %d\n", training_rows);
    const int eval_rows = rows * data_split;
    printf("eval rows : %d\n", eval_rows);

    const int bagging_size = training_rows;

    // const int forest_size = 11;
        


    random_forest * forest = load_forest("./GOOD_FOREST_MODELS/best_model.json");
    // random_forest * forest = buildForest(data, training_rows, cols, forest_size, MAX_DEPTH, bagging_size);
            
    int vote = 0;

    for(int i = training_rows; i < rows; i++){
        
        const double * sample = data[i];                
        
        const enum EVENT_TYPE prediction = predictForest(forest, sample);
        
        printf("forest says %d , answer is %d\n", (int)prediction, (int)sample[cols -1]);
        
        if((int)prediction == (int)sample[cols-1]) vote++;
        
    }


    printf("PREDICTION SCORE : %.2f% (%d/%d)\n",((float)vote/(float)eval_rows) * 100, vote, eval_rows);

    freeForest(forest);       

    for(int i = 0; i < rows; i++){
        
        free(data[i]);
        
    }
    
    free(data);



    // process_folder("./data/csv/", "eeg_blinks.csv");

    return 0;
}


