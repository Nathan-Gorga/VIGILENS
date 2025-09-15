#ifndef CSV_LIB_H
#define CSV_LIB_H

#include "../../globaldefinition.h"
#include <csv.h>

#define MAX_ROWS 200
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