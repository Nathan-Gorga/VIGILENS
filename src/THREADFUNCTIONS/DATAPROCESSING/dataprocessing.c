#include "dataprocessing.h"

void cleanupDataProcessing(void * arg){//BUG : this gets called randomly on cancel it seems
    
    (void)write(STDOUT_FILENO, "Cancel signal received\nCleaned up DATA PROCESSING thread\n", 57);    

    //TODO : implement
   
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