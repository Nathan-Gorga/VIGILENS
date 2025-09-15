#include "csv_lib.h"
#include "random_forest.h"

static void field_cb(void * s, size_t len, void * data){
    CSVData * csvdata = (CSVData *)data;

    if(csvdata->row < MAX_ROWS && csvdata->col < MAX_COLS){
        csvdata->matrix[csvdata->row][csvdata->col] = malloc(len + 1);
        memcpy(csvdata->matrix[csvdata->row][csvdata->col], s, len);
        csvdata->matrix[csvdata->row][csvdata->col][len] = '\0';
        csvdata->col++;
    }

    if(csvdata->col + 1 > csvdata->max_cols){
        csvdata->max_cols = csvdata->col + 1;
    }
}

static void row_cb(int c, void *data){
    CSVData *csvdata = (CSVData *)data;

    if(csvdata->row == 0){
        csvdata->col = 0;
        csvdata->row++;
        return;
    }

    csvdata->row++;
    csvdata->col = 0;
}



static char ***load_csv(const char *filename, size_t * out_rows, size_t *out_cols){
    
    struct csv_parser parser;

    FILE *fp;
    char buf[1024];
    size_t bytes_read;

    char ***matrix = malloc(MAX_ROWS * sizeof(char **));

    for(size_t i = 0; i < MAX_ROWS; i++){

        matrix[i] = malloc(MAX_COLS * sizeof(char *));

        for(size_t j = 0; j < MAX_COLS; j++){
            matrix[i][j] = NULL;
        }
    }

    CSVData csvdata = {matrix, 0, 0};

    if(csv_init(&parser, 0) != 0){
        fprintf(stderr, "Failed to initialize parser\n");
        return NULL;
    }

    fp = fopen(filename, "rb");

    if(!fp){
        perror("fopen");
        csv_free(&parser);
        return NULL;
    }

    while((bytes_read = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if(csv_parse(&parser, buf, bytes_read, field_cb, row_cb, &csvdata) != bytes_read) {
            fprintf(stderr, "Parse error: %s\n", csv_strerror(csv_error(&parser)));
            fclose(fp);
            csv_free(&parser);
            return NULL;
        }
    }


    csv_fini(&parser, field_cb, row_cb, &csvdata);

    fclose(fp);

    csv_free(&parser);

    *out_rows = csvdata.row - 1;
    *out_cols = csvdata.max_cols - 1;
    return matrix;

}

static double **convertToNumeric(char ***data, size_t rows, size_t cols){
    
    double ** numeric  = malloc(rows * sizeof(double *));
    // printf("cols : %d : %s\n",cols , data[1][4]);
    for(int i = 0; i < rows; i++){
        numeric[i] = malloc(cols * sizeof(double));
    }

    for(int i = 1; i < rows; i++){
        for(int j = 0; j < cols; j++){

            if(j < cols - 1){
            
                numeric[i-1][j] = atof(data[i][j]);
            
            } else {
                char * species = data[i][j];

                numeric[i - 1][j] = (double)atoi(species);

                // if(strcmp(species, "setosa") == 0){
                //     numeric[i - 1][j] = (double)SETOSA;//TODO : use enum
                // } else if (strcmp(species, "versicolor") == 0){
                //     numeric[i - 1][j] = (double)VERSICOLOR;
                // } else if (strcmp(species, "virginica") == 0){
                //     numeric[i - 1][j] = (double)VIRIGINICA;
                // } else {
                //     numeric[i - 1][j] = (double)NOT_IRIS;
                // }

            }
        }
    }

    return numeric;

}


double ** getNumericData(char * filename, size_t * rows, size_t * cols){

    char ***raw = load_csv(filename, rows, cols);

    if(!raw) return NULL;

    double ** data = convertToNumeric(raw, *rows, *cols);

    for(int i = 0; i < *rows; i++){
    
        for(int j = 0; j < *cols; j++){
    
            free(raw[i][j]);
    
        }
    
        free(raw[i]);
    }

    free(raw);
    
    return data;

}






// int main(void){

//     size_t rows, cols;

//     double ** data = getNumericData("./data/iris.csv", &rows, &cols);


//     for(int i = 0; i < rows - 1; i++){
        

//         for(int j = 0; j < cols ; j++){
            
            
//             printf("%f\t", data[i][j]);
    
//         }
    
//         printf("\n");
//     }


//     for(int i = 0; i < rows; i++){
    
    
//         free(data[i]);
//     }

//     free(data);

//     return 0;
// }
