#include "dataprocessing.h"

struct random_forest * forest = NULL;

static void cleanupHandler(void * event_buffer){

    (void)printf("Cancel signal received\n");

    if(event_buffer != NULL) free((double*)event_buffer);
    if(forest != NULL) freeForest((random_forest*)forest);


    (void)printf("Cleaned up thread\n");

}

static void dataProcessing(void){

    sigset_t set;

    int sig;

    while(sigemptyset(&set));

    while(sigaddset(&set, SIGCONT));

    size_t event_buffer_size = 0;

    double * event_buffer = calloc(MAX_EVENT_SIZE, sizeof(double));

    forest = load_forest(JSON_FILE_NAME);

    pthread_cleanup_push(cleanupHandler, event_buffer);

    {// Send ready signal to master

        MUTEX_LOCK(&ready_lock);

        ready_count++;

        (void)pthread_cond_signal(&ready_cond);

        (void)printf(YELLOW"Thread Ready!\n"RESET);

        MUTEX_UNLOCK(&ready_lock);

    }

    //wait for go signal

    if (sigwait(&set, &sig) == 0) {

        (void)printf(YELLOW"PROCESSING RECEIVED GO SIGNAL\n"RESET);

    } else {

        (void)printf("Error waiting for go signal\n");

        pthread_exit(NULL);
    }

    printf(YELLOW"PROCESSING ENTERING MAIN LOOP\n"RESET);

    while(1){

        pthread_testcancel();

        event_buffer_size = getEvent(event_buffer);

        if(event_buffer_size <= 0) continue; // buffer is empty
        
        const event_features signal_features = featureExtraction(event_buffer,event_buffer_size);

        double sample[] = {
            signal_features.slope_1,
            signal_features.slope_2,
            signal_features.slope_3,
            signal_features.t1,
            signal_features.t2,
            signal_features.t3,
            signal_features.std_dev,
            signal_features.sample_entropy,
        };

        enum EVENT_TYPE prediction = predictForest((random_forest*)forest, sample);

        if(prediction == BLINK){
            printf(GREEN"BLINK!\n"RESET);
        } else {
            printf(RED"NOT BLINK!\n"RESET);

        }

        event_buffer_size = 0;
    
    }

    pthread_cleanup_pop(1);
}



void * launchDataProcessing(void * _){

    (void)_; // compilation warning : unused parameter

    dataProcessing();

    pthread_exit(NULL);
}




