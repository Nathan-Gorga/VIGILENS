#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "../../globaldefinition.h"
#include "../../RINGBUFFER/ringbuffer.h"
#include "../../EVENTDATASTRUCTURES/eventdatastructure.h"
#include "../../LOG/log.h"
#include "../../ALGOS/THRESHOLD/threshold_algos.h"
#include "../../ALGOS/RANDOM_FOREST/build_forest.h"
#include "../../ALGOS/RANDOM_FOREST/predict_forest.h"
#include "../../ALGOS/RANDOM_FOREST/event_feature.h"

#define JSON_FILE_NAME "./ALGOS/RANDOM_FOREST/GOOD_FOREST_MODELS/best_model.json"


void * launchDataProcessing(void * _);

extern pthread_mutex_t ready_lock;

extern pthread_cond_t ready_cond;

extern int ready_count;

#endif


