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

enum EVENT_TYPE predictForest(random_forest * f, const double * sample){

    if(f == NULL) return NOT_BLINK;//OTHER;


    int vote[NUM_EVENT_TYPE] = {0};

    for(int i = 0; i < f->size; i++){

        // printf("Querying tree %d\n",i+1);

        const int idx = predict(f->forest[i], sample);

        if(idx != -1) vote[idx]++;

    }  

    // char ** str = {"Not Blink", "Blink"};
    // printf("\n");

    // for(int i = 0; i < NUM_EVENT_TYPE; i++){
    //     printf("%d voted for %d\n", vote[i], i);
    // }
    // printf("\n");

    return iarrmax(vote, NUM_EVENT_TYPE);

}

