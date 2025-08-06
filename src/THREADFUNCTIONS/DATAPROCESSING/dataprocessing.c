#include "dataprocessing.h"

#define printf(...) printf(DATA_PROCESSING_TEXT_COLOR"DATA PROCESSING:%d - ",__LINE__); printf(__VA_ARGS__); printf(RESET)

static void cleanupHandler(void * event_buffer){

    (void)printf("Cancel signal received\n");

    if(event_buffer != NULL){
        free((float*)event_buffer);
    }

    (void)printf("Cleaned up thread\n");

}


void * launchDataProcessing(void * arg){

    dataProcessing();

    pthread_exit(NULL);
}


static void dataProcessing(void){

    (void)printf("Thread launched succesfully\n");

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

        (void)printf("Received SIGCONT, continuing execution.\n");

    } else {

        (void)printf("Error waiting for go signal\n");

        pthread_exit(NULL);
    }

    (void)printf("Entering main loop\n");

    while(1){

        pthread_testcancel();

        event_buffer_size = getEvent(event_buffer);//TODO : test the handling of the event

        if(event_buffer_size > 0){//there is an event

           logEntry(THREAD_DATA_PROCESSING, LOG_INFO, "got event from event buffer");

	   if(simpleThresholdEventDetection(10.0f, event_buffer, event_buffer_size)){

	    	printf("FOUND AN EVENT\n");

	   }

            event_buffer_size = 0;
        }
    }

    pthread_cleanup_pop(1);
}
