#ifndef CSV_STUFF_H
#define CSV_STUFF_H

#include "../../globaldefinition.h"
#include "build_forest.h"
#include "event_feature.h"
#include <csv.h>
#include <dirent.h>

#define MAX_ROWS 1000
#define MAX_COLS 10
#define MAX_FIELD_LEN 64

typedef struct {
    char ***matrix;
    size_t row;
    size_t col;
    size_t max_cols;
} CSVData;

double ** getNumericData(char * filename, size_t * rows, size_t * cols);

#endif