#ifndef BUILD_FOREST_H
#define BUILD_FOREST_H

#include "../../globaldefinition.h"
#include <float.h>
#include "csv_stuff.h"
#include "event_feature.h"

#define MIN_SAMPLES 3
#define MAX_DEPTH 12


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


typedef struct random_struct{

    size_t size;
    size_t max_depth;

    tree_node ** forest;

} random_forest;




tree_node * createNode(const enum NODE_TYPE type, const int feature_index, const double threshold, const enum EVENT_TYPE class_label);

void printNode(tree_node * n);



bool sameClass(double ** data, const size_t rows, const size_t cols);

enum EVENT_TYPE majorityClass(double ** data,const size_t rows,const size_t cols);

tree_node * buildTree(double ** data, const size_t rows, const size_t cols, const size_t depth);


void freeTree(tree_node * n);

void printTree(const tree_node *node, int depth);


double ** bagging(double **data, const size_t rows, const size_t cols, const size_t iterations);

random_forest * createForest(const size_t size, const size_t max_depth);

void freeForest(random_forest * f);



random_forest * buildForest(
    double ** data, 
    const size_t rows, 
    const size_t cols, 
    const size_t forest_size,
    const size_t forest_depth,
    const size_t bagging_size
);



#endif

