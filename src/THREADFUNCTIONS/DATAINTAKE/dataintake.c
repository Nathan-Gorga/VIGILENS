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
    
    sigset_t set;
    int sig;

    while(sigemptyset(&set));
    while(sigaddset(&set, SIGCONT));


    struct ring_buffer * internal_ring_buffer = initRingBuffer(INTERNAL_RING_BUFFER_SIZE, INTERNAL_RING_BUFFER);
    
    if(internal_ring_buffer == NULL) exit(EXIT_FAILURE);
    
    pthread_cleanup_push(cleanupHandler, internal_ring_buffer);


     {// Send ready signal to master
        if(pthread_mutex_lock(&ready_lock) != 0){
            (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__);
            pthread_exit(NULL);
        }
    
        ready_count++;
    
        (void)pthread_cond_signal(&ready_cond);
        
        (void)printf("Thread Ready!\n");
    
        if(pthread_mutex_unlock(&ready_lock) != 0){
            (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__);
            pthread_exit(NULL);
        }
    }

    //wait for go signal
    if(sigwait(&set, &sig) == 0) {
        (void)printf("Received SIGCONT, continuing execution.\n");
        
    }else {
        (void)printf("Error waiting for go signal\n");
        pthread_exit(NULL);
    }


    //TODO : sends go signal to data stream source

    (void)printf("Entering main loop\n");

    while(1){
    //TODO : receive data 
        
    //TODO : put data in internal ring buffer

    //TODO : check if there is a leave from baseline in the ring buffer(how big should be the buffer that's checked and shoudl we send the whole buffer or bytes at a time?)

    //TODO : return to data intake
    
        pthread_testcancel();
    }

    pthread_cleanup_pop(1);
}

