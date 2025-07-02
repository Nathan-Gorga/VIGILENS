#include "dataprocessing.h"

static void cleanupHandler(void * arg){
    
    (void)printf("Cancel signal received\n");    


    //TODO : implement


    (void)printf("Cleaned up thread\n");    

}


void * launchDataProcessing(void * arg){
    
    dataProcessing();

    pthread_exit(NULL);
}

static void dataProcessing(void){
    pthread_cleanup_push(cleanupHandler, NULL);

    (void)printf("Thread launched succesfully\n");

    float ** eventBuffer;
    while(1){
        
        if(getEvent(eventBuffer) > 0){//there is an event

            //TODO : implement

            free(eventBuffer);
        }
    }


    pthread_cleanup_pop(1);
}