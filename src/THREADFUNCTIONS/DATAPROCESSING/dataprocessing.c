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
    float * event_buffer = NULL;

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
        
        event_buffer = getEvent(&event_buffer_size);

        if(event_buffer_size > 0){//there is an event
            PRINTF_DEBUG

            printf("Got event of size %d\n", event_buffer_size);
            PRINTF_DEBUG

            // for(int i = 0; i < event_buffer_size; i++){
            //     printf("%f\n", event_buffer[i]);
            // }
            PRINTF_DEBUG

            //TODO : implement

            free(event_buffer);
            PRINTF_DEBUG
            event_buffer = NULL;
            event_buffer_size = 0;
            PRINTF_DEBUG
        }

        pthread_testcancel();
    }


    pthread_cleanup_pop(1);
}