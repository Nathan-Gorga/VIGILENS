#include "predict_forest.h"



enum EVENT_TYPE predict(const tree_node *node, const double *sample) {
    if (node == NULL) {
        // safety fallback
        return NOT_BLINK;//OTHER;
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


enum EVENT_TYPE predictForest(random_forest * f, const double * sample){

    if(f == NULL) return NOT_BLINK;//OTHER;


    int vote[NUM_EVENT_TYPE] = {0};

    for(size_t i = 0; i < f->size; i++){

        // printf("Querying tree %d\n",i+1);

        const int idx = predict(f->forest[i], sample);

        if(idx != -1) vote[idx]++;

    }  


    return iarrmax(vote, NUM_EVENT_TYPE);

}

