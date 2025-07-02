#include "dataintake.h"


void cleanupDataIntake(void * arg){
    
    (void)write(STDOUT_FILENO, "Cancel signal received\nCleaned up DATA INTAKE thread\n", 53);    

    //TODO : implement
   
}


void * launchDataIntake(void * arg){
    
    dataIntake();

    pthread_exit(NULL);
}


void dataIntake(void){
    pthread_cleanup_push(cleanupDataIntake, NULL);

    (void)printf("Thread launched succesfully\n");


    while(1){
        
        pthread_testcancel();
    }


    //TODO : init internal ring buffer
    
    //TODO : send ready signal to master

    //TODO : sends go signal to data stream source

    //TODO : receive data 

    //TODO : put data in internal ring buffer

    //TODO : check if there is a leave from baseline in the ring buffer(how big should be the buffer that's checked and shoudl we send the whole buffer or bytes at a time?)

    //TODO : return to data intake

    //TODO : always take into account that a cancel signal may come from master

    //TODO : free internal ring buffer

    pthread_cleanup_pop(1);
}

