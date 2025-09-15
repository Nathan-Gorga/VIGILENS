#include "test_forest.h"


void benchmarkForest(double ** data, const int rows, const int cols, const float data_split, const int max_forest_size, const int max_loops){

    const int training_rows = rows - (int)(rows * data_split);

    const int eval_rows = rows * data_split;

    const int bagging_size = training_rows;

    for(int forest_size = 1; forest_size <= max_forest_size; forest_size++){
        
        double precision = 0;

        for(int loop = 0; loop < max_loops; loop++){

            random_forest * forest = buildForest(data, training_rows, cols, forest_size, MAX_DEPTH, bagging_size);
            
            int vote = 0;

            for(int i = training_rows; i < rows; i++){
                
                const double * sample = data[i];                
                
                const enum EVENT_TYPE prediction = predictForest(forest, sample);
                
                // printf("forest says : %d, answer is %d\n", prediction, (int)sample[cols - 1]);
                
                if((int)prediction == (int)sample[cols-1]) vote++;
                
            }


            precision += ((double)vote/(double)eval_rows) * 100;
            // printf("PREDICTION SCORE : %.2f% (%d/%d)\n",((float)vote/(float)eval_rows) * 100, vote, eval_rows);


            freeForest(forest);
            
        }
        
        printf("avg precision for forest size %d : %lf\n", forest_size, precision/max_loops);
        
        
    }   

}