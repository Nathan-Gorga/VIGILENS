#ifndef RANDOM_FOREST_H
#define RANDOM_FOREST_H

#include "../../globaldefinitions.h"

typedef struct {

	float * features;

	int label;

} data_point;


typedef struct {

	data_point * data;

	int size;

	int num_features;

} data_set;

typedef struct tree_node {

	int feature_index;

	float threshold;

	int is_leaf;

	int class_label;

	struct tree_node * left;

	struct tree_node * right;

} tree_node;

typedef struct {

	size_t num_trees;

	tree_node * forest;

} random_forest;



tree_node * buildTree(data_point * data, const int size, const int num_features, const int max_depth);

void bootstrapSample(data_set * original, data_set * sampled);

int predictTree(tree_node * tree, float * features);

int predictForest(random_forest * r_forest, float * features);

void freeTree(tree_node * tree);


#endif
