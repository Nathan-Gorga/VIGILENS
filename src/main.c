#include "main.h"

bool keyboard_interrupt = false;

pthread_mutex_t ready_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

int ready_count = 0;

const int THREADS_TO_WAIT = 2;


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

    
    {//wait for ready signal from both threads
        pthread_mutex_lock(&ready_lock);
        while (ready_count < THREADS_TO_WAIT) {
            pthread_cond_wait(&ready_cond, &ready_lock);
        }
        printf(GREEN"All threads are ready\n"RESET);
        pthread_mutex_unlock(&ready_lock); 
    }


    //TODO : send go signal to both threads

    while(!keyboard_interrupt);
    
    printf("Cancelling slave threads\n");
    
    pthread_cancel(data_intake_thread);
    pthread_cancel(data_processing_thread);

    pthread_join(data_intake_thread, NULL);
    pthread_join(data_processing_thread, NULL);

end:
    freeEventDatastructure();
    destroyMutexes();
    pthread_mutex_destroy(&ready_lock);
    pthread_cond_destroy(&ready_cond);

    return 0;
}

