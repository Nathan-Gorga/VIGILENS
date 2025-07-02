#include "main.h"

bool keyboard_interrupt = false;

void handle_sigint(const int sig) {
    printf("Keyboard interrupt received\n");
    keyboard_interrupt = true;
}

int main(void){
   
    signal(SIGINT, handle_sigint);

    pthread_t data_intake_thread, data_processing_thread;

    (void)printf("Starting VIGILENCE SYSTEM\n");

    (void)printf("Initializing event data structure\n");
    initEventDatastructure(EVENT_RING_BUFFER_SIZE);

    (void)printf("Creating mutexes\n");
    createMutexes();

    if(pthread_create(&data_intake_thread, NULL, launchDataIntake, NULL) != 0){
        (void)printf("Error creating data intake thread\n"); goto end;
    }

    if(pthread_create(&data_processing_thread, NULL, launchDataProcessing, NULL) != 0){
        (void)printf("Error creating data processing thread\n"); goto end;
    }
    
    
    while(!keyboard_interrupt);
    
    printf("Cancelling slave threads\n");
    
    pthread_cancel(data_intake_thread);
    pthread_cancel(data_processing_thread);

    pthread_join(data_intake_thread, NULL);
    pthread_join(data_processing_thread, NULL);

end:
    freeEventDatastructure();
    destroyMutexes();

    return 0;
}

