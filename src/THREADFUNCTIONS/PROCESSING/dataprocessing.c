#include "dataprocessing.h"

static void cleanupHandler(void * event_buffer){

    (void)printf("Cancel signal received\n");

    if(event_buffer != NULL){

        free((float*)event_buffer);

    }

    (void)printf("Cleaned up thread\n");

}

static void dataProcessing(void){

    sigset_t set;

    int sig;

    while(sigemptyset(&set));

    while(sigaddset(&set, SIGCONT));

    size_t event_buffer_size = 0;

    float * event_buffer = calloc(MAX_EVENT_SIZE, sizeof(float));

    pthread_cleanup_push(cleanupHandler, event_buffer);

    {// Send ready signal to master

        MUTEX_LOCK(&ready_lock);

        ready_count++;

        (void)pthread_cond_signal(&ready_cond);

        (void)printf("Thread Ready!\n");

        MUTEX_UNLOCK(&ready_lock);

    }

    //wait for go signal

    if (sigwait(&set, &sig) == 0) {

        (void)printf("PROCESSING RECEIVED GO SIGNAL\n");

    } else {

        (void)printf("Error waiting for go signal\n");

        pthread_exit(NULL);
    }

    printf("PROCESSING ENTERING MAIN LOOP\n");

    while(1){

        pthread_testcancel();

        event_buffer_size = getEvent(event_buffer);

        if(event_buffer_size > 0){//there is an event

            if(simpleThresholdEventDetection(5.0f, event_buffer, event_buffer_size)){

                // printf("FOUND AN EVENT\n");

            }

            event_buffer_size = 0;
        }
    }

    pthread_cleanup_pop(1);
}



void * launchDataProcessing(void * _){

    (void)_; // compilation warning : unused parameter

    dataProcessing();

    pthread_exit(NULL);
}




