#include "dataintake.h"


static void cleanupHandler(void * arg){//BUG : this gets called randomly on cancel it seems
    
    (void)printf("Cancel signal received\n");    


    //TODO : implement


    (void)printf("Cleaned up thread\n");    
   
}


void * launchDataIntake(void * arg){
    
    dataIntake();

    pthread_exit(NULL);
}


static void dataIntake(void){
    pthread_cleanup_push(cleanupHandler, NULL);

    (void)printf("Thread launched succesfully\n");

    struct ring_buffer * internal_ring_buffer = initRingBuffer(INTERNAL_RING_BUFFER_SIZE, INTERNAL_RING_BUFFER);

    if(internal_ring_buffer == NULL) pthread_exit(NULL);


    //TODO : send ready signal to master

    //TODO : sends go signal to data stream source

    //TODO : receive data 

    //TODO : put data in internal ring buffer

    //TODO : check if there is a leave from baseline in the ring buffer(how big should be the buffer that's checked and shoudl we send the whole buffer or bytes at a time?)

    //TODO : return to data intake

    //TODO : always take into account that a cancel signal may come from master

    //TODO : free internal ring buffer

    freeRingBuffer(internal_ring_buffer);

    pthread_cleanup_pop(1);
}

