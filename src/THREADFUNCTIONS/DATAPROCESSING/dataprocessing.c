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
    
    sigset_t set;
    int sig;

    while(sigemptyset(&set));
    while(sigaddset(&set, SIGCONT));

    size_t event_buffer_size;
    float ** event_buffer = NULL;

    pthread_cleanup_push(cleanupHandler, event_buffer);

    {// Send ready signal to master
        MUTEX_LOCK(&ready_lock);
    
        ready_count++;
    
        (void)pthread_cond_signal(&ready_cond);

        (void)printf("Thread Ready!\n");
    
        MUTEX_UNLOCK(&ready_lock);
    }


    //wait for go signal
    if(sigwait(&set, &sig) == 0) {
        
        (void)printf("Received SIGCONT, continuing execution.\n");
        
    }else {
        (void)printf("Error waiting for go signal\n");
        pthread_exit(NULL);
    }

    (void)printf("Entering main loop\n");
    while(1){
        
        event_buffer_size = getEvent(event_buffer);

        if(event_buffer_size > 0){//there is an event


            //TODO : implement

            free(event_buffer);
            event_buffer = NULL;
        }
        pthread_testcancel();
    }


    pthread_cleanup_pop(1);
}