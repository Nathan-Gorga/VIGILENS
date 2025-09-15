#ifndef TEST_FOREST_H
#define TEST_FOREST_H

#include "../../globaldefinition.h"
#include "build_forest.h"
#include "predict_forest.h"
#include "event_feature.h"

void benchmarkForest(double ** data, const int rows, const int cols, const float data_split, const int max_forest_size, const int max_loops);


#endif