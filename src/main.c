#include "main.h"


volatile bool keyboard_interrupt = false;

pthread_mutex_t ready_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

int ready_count = 0;

// increase this number if you want to launch and synchronise more threads at the start
const int THREADS_TO_WAIT = 2;

void handle_sigint(const int sig) {

    (void)printf("Keyboard interrupt(%d) received\n",sig);

    keyboard_interrupt = true;
}

static bool syncThreads(pthread_t * data_intake_thread, pthread_t * data_processing_thread) {

    {//wait for ready signal from both threads

        if(pthread_mutex_lock(&ready_lock) != 0){

            (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__); 
            
            return false;
        }

        while (ready_count < THREADS_TO_WAIT) {

            (void)pthread_cond_wait(&ready_cond, &ready_lock);

        }

        if(pthread_mutex_unlock(&ready_lock) != 0){

            (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__);             

            return false;
        }
    }

    // send CONTINUE signal to both threads
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
    
    sigset_t set;
    
    while(sigemptyset(&set));
    
    while(sigaddset(&set, SIGCONT));
    
    // Block SIGCONT in all threads, necessary for sigwait to work properly
    while(pthread_sigmask(SIG_BLOCK, &set, NULL));

    initEventDatastructure(EVENT_RING_BUFFER_SIZE);

    if(createMutexes() != 0){

        (void)printf("Error creating mutexes\n"); return false;
    }

    if(pthread_create(data_intake_thread, NULL, launchDataIntake, NULL) != 0){

        (void)printf("Error creating data intake thread\n"); return false;
    }

    if(pthread_create(data_processing_thread, NULL, launchDataProcessing, NULL) != 0){

        (void)printf("Error creating data processing thread\n"); return false;
    }

    return syncThreads(data_intake_thread, data_processing_thread);

}


int main(void){

    //STARTUP
    pthread_t data_intake_thread, data_processing_thread;

    if(!startupFunction(&data_intake_thread, &data_processing_thread)) goto end;

    //WAIT LOOP
    while(!keyboard_interrupt) usleep(100);

    //CLOSE
    (void)pthread_cancel(data_intake_thread);

    (void)pthread_cancel(data_processing_thread);

    (void)pthread_join(data_intake_thread, NULL);

    (void)pthread_join(data_processing_thread, NULL);

    
    //CLEANUP
    end:

    freeEventDatastructure();

    (void)destroyMutexes();

    #ifdef UART_ENABLED

        endUART();

    #endif

    (void)usleep(500);

    (void)closeLoggingSystem();

    return 0;
}

