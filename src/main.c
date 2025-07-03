#include "main.h"


bool keyboard_interrupt = false;

pthread_mutex_t ready_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

int ready_count = 0;

const int THREADS_TO_WAIT = 2;


void handle_sigint(const int sig) {
    (void)printf("Keyboard interrupt received\n");
    keyboard_interrupt = true;
}

int main(void){
   
    (void)signal(SIGINT, handle_sigint);

    sigset_t set;

    while(sigemptyset(&set));

    while(sigaddset(&set, SIGCONT));
    
    // Block SIGCONT in all threads, necessary for sigwait to work properly
    while(pthread_sigmask(SIG_BLOCK, &set, NULL));

    pthread_t data_intake_thread, data_processing_thread;
    (void)printf("Starting VIGILENCE SYSTEM\n");

    (void)printf("Initializing event data structure\n");
    initEventDatastructure(EVENT_RING_BUFFER_SIZE);

    (void)printf("Creating mutexes\n");
    if(createMutexes() != 0){
        (void)printf("Error creating mutexes\n"); goto end;
    }

    if(pthread_create(&data_intake_thread, NULL, launchDataIntake, NULL) != 0){
        (void)printf("Error creating data intake thread\n"); goto end;
    }

    if(pthread_create(&data_processing_thread, NULL, launchDataProcessing, NULL) != 0){
        (void)printf("Error creating data processing thread\n"); goto end;
    }

    
    {//wait for ready signal from both threads
        if(pthread_mutex_lock(&ready_lock) != 0){
            (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__); goto end;
        }

        while (ready_count < THREADS_TO_WAIT) {
            (void)pthread_cond_wait(&ready_cond, &ready_lock);
        }

        (void)printf(GREEN"All threads are ready\n"RESET);

        if(pthread_mutex_unlock(&ready_lock) != 0){
            (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__); goto end;
        } 
    }

    (void)printf("Sending go signal to slave threads\n");
    
    while(pthread_kill(data_intake_thread, SIGCONT));
    while(pthread_kill(data_processing_thread, SIGCONT));

    while(!keyboard_interrupt);
    
    (void)printf("Cancelling slave threads\n");

    (void)pthread_cancel(data_intake_thread);
    (void)pthread_cancel(data_processing_thread);

    (void)pthread_join(data_intake_thread, NULL);
    (void)pthread_join(data_processing_thread, NULL);

end:

    freeEventDatastructure();
    (void)destroyMutexes();

    return 0;
}

