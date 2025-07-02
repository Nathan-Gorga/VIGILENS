#include "dataprocessing.h"

static void cleanupHandler(void * eventBuffer){
    
    (void)printf("Cancel signal received\n");    

    if(eventBuffer != NULL){
        free((float*)eventBuffer);
    }

    (void)printf("Cleaned up thread\n");    

}


void * launchDataProcessing(void * arg){
    
    dataProcessing();

    pthread_exit(NULL);
}

static void dataProcessing(void){
    
    (void)printf("Thread launched succesfully\n");
    
    float ** event_buffer;
    
    pthread_cleanup_push(cleanupHandler, event_buffer);
    
    while(1){
        
        if(getEvent(event_buffer) > 0){//there is an event

            //TODO : implement

            free(event_buffer);
            event_buffer = NULL;
        }
    }


    pthread_cleanup_pop(1);
}