#include "csv_stuff.h"

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

static void row_cb(int _, void *data){
    
    (void)_;

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

    CSVData csvdata = {matrix, 0, 0, MAX_COLS};

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
    for(size_t i = 0; i < rows; i++){
        numeric[i] = malloc(cols * sizeof(double));
    }

    for(size_t i = 1; i < rows; i++){
        for(size_t j = 0; j < cols; j++){

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

    for(size_t i = 0; i < *rows; i++){
    
        for(size_t j = 0; j < *cols; j++){
    
            free(raw[i][j]);
    
        }
    
        free(raw[i]);
    }

    free(raw);
    
    return data;

}




// Reads a CSV file with two columns: time, value
// Returns array of doubles with values, sets *out_len
double* read_csv_values(const char* filename, int* out_len) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("File open failed");
        *out_len = 0;
        return NULL;
    }

    char line[256];
    int capacity = 100;
    int count = 0;
    double* values = malloc(capacity * sizeof(double));
    if (!values) {
        perror("Memory allocation failed");
        fclose(fp);
        *out_len = 0;
        return NULL;
    }

    // Skip header line
    fgets(line, sizeof(line), fp);

    // Read each line
    double time, val;
    while (fscanf(fp, "%lf,%lf", &time, &val) == 2) {
        if (count >= capacity) {
            capacity *= 2;
            double* tmp = realloc(values, capacity * sizeof(double));
            if (!tmp) {
                perror("Realloc failed");
                free(values);
                fclose(fp);
                *out_len = 0;
                return NULL;
            }
            values = tmp;
        }
        values[count++] = val;
    }

    fclose(fp);
    *out_len = count;
    return values;
}




// Write header once
void write_csv_header(FILE *fp) {
    fprintf(fp,
        "slope1,slope2,slope3,"
        "t1,t2,t3,"
        "std_dev,sample_entropy,label\n"
    );
}

void process_folder(const char *foldername, const char *outfile) {
    DIR *dir;
    struct dirent *entry;
    FILE *fp_out = fopen(outfile, "w");
    if (!fp_out) {
        perror("Failed to open output CSV");
        return;
    }

    write_csv_header(fp_out);

    dir = opendir(foldername);
    if (!dir) {
        perror("Failed to open folder");
        fclose(fp_out);
        return;
    }

    char filepath[1024];
    while ((entry = readdir(dir)) != NULL) {
        // skip . and ..
        if (entry->d_name[0] == '.') continue;

        // only process .csv files
        const char *ext = strrchr(entry->d_name, '.');
        if (!ext || strcmp(ext, ".csv") != 0) continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", foldername, entry->d_name);

        int len;
        double *vals = read_csv_values(filepath, &len);
        if (!vals || len == 0) {
            fprintf(stderr, "Skipping %s (empty or error)\n", filepath);
            free(vals);
            continue;
        }

        event_features feat = featureExtraction(vals, len);

        int label = BLINK;
        if(strstr(entry->d_name, "not_blink")){
            label = NOT_BLINK;
        }

        // write row
        fprintf(fp_out, "%f,%f,%f,%d,%d,%d,%f,%f,%d\n",
            feat.slope_1, feat.slope_2, feat.slope_3,
            feat.t1, feat.t2, feat.t3,
            feat.std_dev, feat.sample_entropy,
            label
        );

        free(vals);
    }

    closedir(dir);
    fclose(fp_out);
}



// Recursive helper to write JSON for a node
void write_node_json(FILE *f, tree_node *node) {
    if (!node) {
        fprintf(f, "null");
        return;
    }

    fprintf(f, "{\n");
    fprintf(f, "\t\"type\": \"%d\",\n", node->type);
    fprintf(f, "\t\"feature_index\": \"%d\",\n", node->feature_index);
    fprintf(f, "\t\"threshold\": \"%.6f\",\n", node->threshold);
    fprintf(f, "\t\"label\": \"%d\",\n", node->label);

    fprintf(f, "\t\"left\": ");
    write_node_json(f, node->left);
    fprintf(f, ",\n");

    fprintf(f, "\t\"right\": ");
    write_node_json(f, node->right);

    fprintf(f, "}\n");
}

void write_forest_struct_json(FILE * f, random_forest * forest){
    if(!forest){
        fprintf(f, "null");
        return;
    }

    // fprintf(f, "{\n");
    fprintf(f, "\t\"max_depth\": \"%zu\",\n", forest->max_depth);
    fprintf(f, "\t\"size\": \"%zu\",\n", forest->size);
    // fprintf(f, "},\n");    
    fprintf(f, "\t\"forest\":");


}

// Main export function
int save_forest_json(const char *filename, struct random_forest *forest) {

    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Failed to open file");
        return -1;
    }
    

    fprintf(f, "{");

    write_forest_struct_json(f, forest);
    
    fprintf(f, "[");
    for(size_t i = 0; i < ((random_forest*)forest)->size; i++){

        write_node_json(f, ((random_forest*)forest)->forest[i]);

        if(i < ((random_forest*)forest)->size - 1)fprintf(f, ",");

    }

    fprintf(f, "]");

    fprintf(f, "}");



    fclose(f);
    return 0;
}

// Recursive function to build a tree node from JSON object
static tree_node * build_tree(cJSON *node_json) {
    if (!node_json) return NULL;

    tree_node *node = malloc(sizeof(tree_node));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    node->type = atoi(cJSON_GetObjectItem(node_json, "type")->valuestring);
    node->feature_index = atoi(cJSON_GetObjectItem(node_json, "feature_index")->valuestring);
    node->threshold = atof(cJSON_GetObjectItem(node_json, "threshold")->valuestring);
    node->label = atoi(cJSON_GetObjectItem(node_json, "label")->valuestring);

    cJSON *left = cJSON_GetObjectItem(node_json, "left");
    cJSON *right = cJSON_GetObjectItem(node_json, "right");

    node->left = (left && !cJSON_IsNull(left)) ? build_tree(left) : NULL;
    node->right = (right && !cJSON_IsNull(right)) ? build_tree(right) : NULL;

    return node;
}

// Load forest from JSON file
struct random_forest* load_forest(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    char *json_str = malloc(fsize + 1);
    fread(json_str, 1, fsize, fp);
    fclose(fp);
    json_str[fsize] = '\0';

    cJSON *json = cJSON_Parse(json_str);
    if (!json) {
        fprintf(stderr, "JSON parsing error\n");
        free(json_str);
        return NULL;
    }

    random_forest *forest = malloc(sizeof(random_forest));
    forest->max_depth = atoi(cJSON_GetObjectItem(json, "max_depth")->valuestring);
    forest->size = atoi(cJSON_GetObjectItem(json, "size")->valuestring);

    cJSON *trees_json = cJSON_GetObjectItem(json, "forest");
    forest->forest = malloc(forest->size * sizeof(tree_node*));

    int i = 0;
    cJSON *tree_json;
    cJSON_ArrayForEach(tree_json, trees_json) {
        forest->forest[i++] = build_tree(tree_json);
    }

    cJSON_Delete(json);
    free(json_str);

    return (struct random_forest*)forest;
}

// Example traversal
void print_tree(tree_node *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("Type:%d Feature:%d Thresh:%.6f Label:%d\n",
           node->type, node->feature_index, node->threshold, node->label);
    print_tree(node->left, depth + 1);
    print_tree(node->right, depth + 1);
}

// int main() {
//     RandomForest *forest = load_forest("good_forest_2.json");
//     if (!forest) return 1;

//     printf("Loaded forest with %d trees (max depth %d)\n", forest->size, forest->max_depth);
//     for (int i = 0; i < forest->size; i++) {
//         printf("Tree %d:\n", i);
//         print_tree(forest->trees[i], 0);
//     }

//     // TODO: free memory
//     return 0;
// }

