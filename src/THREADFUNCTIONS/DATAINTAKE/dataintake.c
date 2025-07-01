#include "dataintake.h"


void * launchDataIntake(void * arg){
    
    dataIntake();
    
    pthread_exit(NULL);
}


void dataIntake(void){
    
}