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


    //TODO : sends go signal to data stream source

    (void)printf("Entering main loop\n");

    float mockEvent[5];

    
    for(int i = 0; i < 5; i++){
       mockEvent[i] = i + 1; 
    }   

    
    while(1){
    usleep(1000*1000);
    //TODO : receive data 
    PRINTF_DEBUG
    printf("Adding event of size %d\n", 5);
    addEvent(mockEvent, 5);
    PRINTF_DEBUG

    printf("testing event\n");
    // testEventDatastructure();
    PRINTF_DEBUG

    //TODO : put data in internal ring buffer

    //TODO : check if there is a leave from baseline in the ring buffer(how big should be the buffer that's checked and shoudl we send the whole buffer or bytes at a time?)

    //TODO : return to data intake
    
        pthread_testcancel();
    }

    pthread_cleanup_pop(1);
}

