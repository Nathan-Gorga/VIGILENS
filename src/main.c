#include "main.h"

#define printf(...) printf(MAIN_TEXT_COLOR"MASTER:%d - ",__LINE__); printf(__VA_ARGS__); printf(RESET)


volatile bool keyboard_interrupt = false;

pthread_mutex_t ready_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

int ready_count = 0;

const int THREADS_TO_WAIT = 2;


void handle_sigint(const int sig) {//DONTTOUCH
    PRINTF_DEBUG

    (void)printf("Keyboard interrupt(%d) received\n",sig);
    PRINTF_DEBUG

    keyboard_interrupt = true;
}


static bool syncThreads(pthread_t * data_intake_thread, pthread_t * data_processing_thread) {

    {//wait for ready signal from both threads

        log(THREAD_MASTER, LOG_INFO, "Waiting for slave threads to be ready...");

        if(pthread_mutex_lock(&ready_lock) != 0){
            (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__); 
            log(THREAD_MASTER, LOG_ERROR, "Error when locking ready mutex");

            return false;
        }

        

        while (ready_count < THREADS_TO_WAIT) {
            (void)pthread_cond_wait(&ready_cond, &ready_lock);
        }
        log(THREAD_MASTER, LOG_INFO, "Slave threads are ready");

        (void)printf("All threads are ready\n");

        if(pthread_mutex_unlock(&ready_lock) != 0){
            (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__); 
            log(THREAD_MASTER, LOG_ERROR, "Error when unlocking ready mutex");

            return false;
        } 
    }

    (void)printf("Sending go signal to slave threads\n");
    
    while(pthread_kill(*data_intake_thread, SIGCONT));
    while(pthread_kill(*data_processing_thread, SIGCONT));

    return true;
}


static bool startupFunction(pthread_t * data_intake_thread, pthread_t * data_processing_thread) {
    
    srand(time(NULL));
    
    if(initLoggingSystem() != 0){
        (void)printf("Error initializing logging system\n"); return false;
    }
    
    (void)signal(SIGINT, handle_sigint);
    
    // beginUART(); TODO : uncomment
    // log(THREAD_MASTER, LOG_INFO, "UART initialized");

    
    sigset_t set;
    
    while(sigemptyset(&set));
    
    while(sigaddset(&set, SIGCONT));
    
    // Block SIGCONT in all threads, necessary for sigwait to work properly
    while(pthread_sigmask(SIG_BLOCK, &set, NULL));
    log(THREAD_MASTER, LOG_INFO, "Signal handler initialized");

    (void)printf("Starting VIGILENCE SYSTEM\n");
    log(THREAD_MASTER, LOG_INFO, "STARTING VIGILENCE SYSTEM");


    (void)printf("Initializing event data structure\n");
    initEventDatastructure(12);
    log(THREAD_MASTER, LOG_INFO, "Event data structure initialized");


    (void)printf("Creating mutexes\n");
    if(createMutexes() != 0){
    
        log(THREAD_MASTER, LOG_ERROR, "mutexes failed to initialize");

        (void)printf("Error creating mutexes\n"); return false;
    }
    log(THREAD_MASTER, LOG_INFO, "Succesful creation of mutexes");


    if(pthread_create(data_intake_thread, NULL, launchDataIntake, NULL) != 0){

        log(THREAD_MASTER, LOG_ERROR, "Failed to create data intake thread");

        (void)printf("Error creating data intake thread\n"); return false;
    }

    log(THREAD_MASTER, LOG_INFO, "Succesful creation of data intake thread");


    if(pthread_create(data_processing_thread, NULL, launchDataProcessing, NULL) != 0){
        log(THREAD_MASTER, LOG_ERROR, "Failed to create data processing thread");

        (void)printf("Error creating data processing thread\n"); return false;
    }

    log(THREAD_MASTER, LOG_INFO, "Succesful creation of data processing thread");

    return syncThreads(data_intake_thread, data_processing_thread);

}



int main(void){
    
    pthread_t data_intake_thread, data_processing_thread;

    if(!startupFunction(&data_intake_thread, &data_processing_thread)) goto end;
   
    while(!keyboard_interrupt) usleep(100);
    
    (void)printf("Cancelling slave threads\n");

    PRINTF_DEBUG
    (void)pthread_cancel(data_intake_thread);
    PRINTF_DEBUG

    (void)pthread_cancel(data_processing_thread);
    PRINTF_DEBUG

    (void)pthread_join(data_intake_thread, NULL);
    PRINTF_DEBUG

    (void)pthread_join(data_processing_thread, NULL);
    PRINTF_DEBUG


end:

    freeEventDatastructure();
    (void)destroyMutexes();
    (void)closeLoggingSystem();
    // endUART(); TODO : uncomment

    return 0;
}

