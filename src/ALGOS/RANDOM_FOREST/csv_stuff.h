#ifndef CSV_STUFF_H
#define CSV_STUFF_H

#include <csv.h>
#include <dirent.h>
#include "../../globaldefinition.h"
#include "build_forest.h"
#include "cjson/cJSON.h"  // You need cJSON library
#include "event_feature.h"



typedef struct {
    char ***matrix;
    size_t row;
    size_t col;
    size_t max_cols;
} CSVData;

double ** getNumericData(char * filename, size_t * rows, size_t * cols);

struct random_forest * load_forest(const char *filename);

int save_forest_json(const char *filename, struct random_forest *forest);

#endif

