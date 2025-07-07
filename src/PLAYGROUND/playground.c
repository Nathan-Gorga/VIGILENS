#include "../main.h"


int main(void){
    printf("Hello world\n");

    bool is_not_baseline = true;
    for(int i = 0; i < 10; i++){ 

        is_not_baseline |= !true;//isBaseline(channel_data_point[i], arbitrary_max, arbitrary_min); //TESTME

    }

    printf("%s\n", is_not_baseline ? "true" : "false");


    return 0;
}