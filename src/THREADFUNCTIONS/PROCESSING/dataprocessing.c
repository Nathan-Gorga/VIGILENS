#include "dataprocessing.h"

struct random_forest * forest = NULL;

static void cleanupHandler(void * event_buffer){

    (void)printf("Cancel signal received\n");

    if(event_buffer != NULL) free((double*)event_buffer);
    if(forest != NULL) freeForest((random_forest*)forest);


    (void)printf("Cleaned up thread\n");

}

//FIXME : only works for two channels for now
int separateChannels(double * data, const int size, double * channel1, double * channel2){

    const int new_size = size / 2;

    for(int i = 0; i < new_size; i++){
        channel1[i] = data[i * 2];
        channel2[i] = data[(i * 2) + 1];
    }

    return new_size;
}


static void dataProcessing(void){

    sigset_t set;

    int sig;

    double channel1[MAX_EVENT_DURATION];
    double channel2[MAX_EVENT_DURATION];

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

        if(event_buffer_size > 0){//there is an event

            // printf(YELLOW"received event \n"RESET);
            for(int i = 0; i < event_buffer_size; i++){
                // printf("%.1lf ", event_buffer[i]);
            }//printf("\n");

            // printf(YELLOW"received event of size : %d\n"RESET, event_buffer_size);

            //BUG : dont need to split data, already done in intake, check again just to be sure
            // const int channel_size = separateChannels(event_buffer, event_buffer_size, channel1, channel2);

            // printf(YELLOW"separated channels in sizes of %d\n"RESET, channel_size);

            const event_features signal_features1 = featureExtraction(event_buffer,event_buffer_size);
            // const event_features signal_features2 = featureExtraction(channel2, channel_size);

            double sample1[] = {
                signal_features1.slope_1,
                signal_features1.slope_2,
                signal_features1.slope_3,
                signal_features1.t1,
                signal_features1.t2,
                signal_features1.t3,
                signal_features1.std_dev,
                signal_features1.sample_entropy,
            };

            printFeatures(signal_features1);

            // double sample2[] = {
            //     signal_features2.slope_1,
            //     signal_features2.slope_2,
            //     signal_features2.slope_3,
            //     signal_features2.t1,
            //     signal_features2.t2,
            //     signal_features2.t3,
            //     signal_features2.std_dev,
            //     signal_features2.sample_entropy,
            // };

            enum EVENT_TYPE prediction1 = predictForest((random_forest*)forest, sample1);
            // enum EVENT_TYPE prediction2 = predictForest((random_forest*)forest, sample2);

            if(prediction1 == BLINK){
                printf(GREEN"BLINK!\n"RESET);
            } else {
                printf(RED"NOT BLINK!\n"RESET);

            }
            // if(prediction2 == BLINK){
            //     printf(YELLOW"FOUND A BLINK ON CHANNEL 2!\n"RESET);
            // }

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




