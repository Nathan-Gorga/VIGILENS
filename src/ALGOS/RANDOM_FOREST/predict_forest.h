#ifndef PREDICT_FOREST_H
#define PREDICT_FOREST_H

#include "../../globaldefinition.h"
#include "build_forest.h"
#include "event_feature.h"


enum EVENT_TYPE predict(const tree_node *node, const double *sample);

enum EVENT_TYPE predictForest(random_forest * f, const double * sample);



#endif