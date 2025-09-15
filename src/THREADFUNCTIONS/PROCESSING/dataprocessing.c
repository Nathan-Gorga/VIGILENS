#include "dataprocessing.h"

random_forest * forest = NULL;

static void cleanupHandler(void * event_buffer){

    (void)printf("Cancel signal received\n");

    if(event_buffer != NULL) free((float*)event_buffer);
    if(forest != NULL) freeForest(forest);


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

    float * event_buffer = calloc(MAX_EVENT_SIZE, sizeof(float));

    forest = load_forest(JSON_FILE_NAME);

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


            const int channel_size = separateChannels(event_buffer, event_buffer_size, channel1, channel2);

            const event_features signal_features1 = featureExtraction(channel1, channel_size);
            const event_features signal_features2 = featureExtraction(channel2, channel_size);

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

            double sample2[] = {
                signal_features2.slope_1,
                signal_features2.slope_2,
                signal_features2.slope_3,
                signal_features2.t1,
                signal_features2.t2,
                signal_features2.t3,
                signal_features2.std_dev,
                signal_features2.sample_entropy,
            };

            enum EVENT_TYPE prediction1 = predictForest(forest, sample1);
            enum EVENT_TYPE prediction2 = predictForest(forest, sample2);

            if(prediction1 == BLINK){
                printf("FOUND A BLINK ON CHANNEL 1!\n");
            }

            if(prediction2 == BLINK){
                printf("FOUND A BLINK ON CHANNEL 2!\n");
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




