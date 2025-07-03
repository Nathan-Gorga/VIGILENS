#include "dataintake.h"


static void cleanupHandler(void * internal_ring_buffer){
    
    (void)printf("Cancel signal received\n");    

    freeRingBuffer((struct ring_buffer *)internal_ring_buffer);

    (void)printf("Cleaned up thread\n");    
   
}

void * launchDataIntake(void * arg){
    
    dataIntake();

    pthread_exit(NULL);
}

static void dataIntake(void){
    
    (void)printf("Thread launched succesfully\n");
    
    struct ring_buffer * internal_ring_buffer = initRingBuffer(INTERNAL_RING_BUFFER_SIZE, INTERNAL_RING_BUFFER);
    
    if(internal_ring_buffer == NULL) exit(EXIT_FAILURE);
    
    pthread_cleanup_push(cleanupHandler, internal_ring_buffer);

     {// Send ready signal to master
        pthread_mutex_lock(&ready_lock);
    
        ready_count++;
    
        pthread_cond_signal(&ready_cond);
        printf("Thread Ready!\n");
    
        pthread_mutex_unlock(&ready_lock);
    }
    


    //TODO : sends go signal to data stream source

    while(1){
    //TODO : receive data 
        
    //TODO : put data in internal ring buffer

    //TODO : check if there is a leave from baseline in the ring buffer(how big should be the buffer that's checked and shoudl we send the whole buffer or bytes at a time?)

    //TODO : return to data intake
    
        pthread_testcancel();
    }

    pthread_cleanup_pop(1);
}

