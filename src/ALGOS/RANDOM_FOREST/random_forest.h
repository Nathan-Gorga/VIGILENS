#ifndef RANDOM_FOREST_H
#define RANDOM_FOREST_H

#include "../../globaldefinition.h"
#include "../../../../random-forest-in-c/csv_lib.h"
#include <dirent.h>
#include <float.h>

typedef struct {

    double slope_1;
    double slope_2;
    double slope_3;
    
    int t1;
    int t2;
    int t3;
    
    double std_dev;

    double sample_entropy;

}event_features;


enum EVENT_TYPE{
    OTHER = -1,
    NOT_BLINK,
    BLINK,
    NUM_EVENT_TYPE
    // SLOW_BLINK,
    // FAST_BLINK,
    // SACCADE
};

enum NODE_TYPE{
    ROOT,
    BRANCH,
    LEAF
};

typedef struct {
    double value;
    int label;
} sample;


typedef struct tree_node {

    enum NODE_TYPE type;    

    struct tree_node *left;
    struct tree_node *right;

    int feature_index;

    double threshold;

    enum EVENT_TYPE label;

} tree_node;


typedef struct{

    size_t size;
    size_t max_depth;

    tree_node ** forest;

} random_forest;



#endif

