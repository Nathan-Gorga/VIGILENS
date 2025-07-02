#include "main.h"


int main(void){
   
    pthread_t data_intake_thread;

    (void)printf("Starting VIGILENCE SYSTEM\n");

    (void)printf("Initializing event data structure\n");
    initEventDatastructure(EVENT_RING_BUFFER_SIZE);

    (void)printf("Creating mutexes\n");
    createMutexes();

    if(pthread_create(&data_intake_thread, NULL, launchDataIntake, NULL) != 0)
        (void)printf("Error creating data intake thread\n"); goto end;



    //TODO : wait for ready signal from data intake

    //TODO : wait for keyboard interupt to cancel data intake thread

    //TODO : cancel data intake
    
end:
    freeEventDatastructure();
    destroyMutexes();

    return 0;
}

