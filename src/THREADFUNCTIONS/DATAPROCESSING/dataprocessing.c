#include "dataprocessing.h"

void cleanupDataProcessing(void * arg){
    
    (void)printf("Cancel signal received\n");    


    //TODO : implement


    (void)printf("Cleaned up thread\n");    

}


void * launchDataProcessing(void * arg){
    
    dataProcessing();

    pthread_exit(NULL);
}

static void dataProcessing(void){
    pthread_cleanup_push(cleanupDataProcessing, NULL);

    (void)printf("Thread launched succesfully\n");


    while(1){
        
        pthread_testcancel();
    }


    pthread_cleanup_pop(1);
}