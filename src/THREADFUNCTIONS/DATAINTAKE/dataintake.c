#include "dataintake.h"


static void masterStartupDialogue(void){

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "In master startup dialogue");

    sigset_t set;

    int sig;

    while(sigemptyset(&set));

    while(sigaddset(&set, SIGCONT));

    // Send ready signal to master
    MUTEX_LOCK(&ready_lock);

    ready_count++;

    (void)pthread_cond_signal(&ready_cond);

    #ifdef PRINTF_ENABLED

    (void)printf("Thread Ready!\n");

    #endif

    MUTEX_UNLOCK(&ready_lock);

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Sent ready signal to master");

    //wait for go signal
    if(sigwait(&set, &sig) == 0) {

        (void)printf("Received SIGCONT, continuing execution.\n");

        (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Received SIGCONT, continuing execution.");

    } else {

        (void)printf("Error waiting for go signal\n");

        (void)logEntry(THREAD_DATA_INTAKE, LOG_ERROR, "Error waiting for go signal");

        pthread_exit(NULL);
    }
}

static void cleanupHandler(void * internal_ring_buffer){

    (void)printf("Cancel signal received\n");

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Cancel signal received");

    if(internal_ring_buffer != NULL) freeRingBuffer((struct ring_buffer *)internal_ring_buffer);

    (void)printf("Cleaned up thread\n");
}

size_t minusTail(const int tail, const int num_data_points){

    return !(tail - num_data_points < 0) ? tail - num_data_points : (tail - num_data_points) + INTERNAL_RING_BUFFER_SIZE;
}

size_t addTail(const int tail, const int num_data_points){

    return !(tail + num_data_points > INTERNAL_RING_BUFFER_SIZE) ? tail + num_data_points : (num_data_points + tail) - INTERNAL_RING_BUFFER_SIZE;

}


#define BLINK_DURATION 0.4f//in seconds


//see if this requires num_channels
#define NUM_SAMPLES_IN_BLINK (size_t)(BLINK_DURATION * SAMPLING_RATE)


static void dataIntake(void){//TESTME : test everything
    
    int data_intake_count = 0;
    
    size_t blink_indices[20];//TESTME : try different sizes

    struct ring_buffer * internal_ring_buffer;

    size_t i, num_data_points = 0;

    size_t start_event, end_event;

    float channel_data_point[PACKET_BUFFER_SIZE] = {0.0f};

    float window_buffer[WINDOW_SIZE];

    #ifdef PRINTF_ENABLED

    (void)printf("Thread launched succesfully\n");

    #endif

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "thread launch successfully");

    internal_ring_buffer = initRingBuffer(INTERNAL_RING_BUFFER_SIZE, INTERNAL_RING_BUFFER);

    setupFilter();

    if(internal_ring_buffer == NULL) {

        (void)logEntry(THREAD_DATA_INTAKE, LOG_ERROR , "failure initializing internal ring buffer");

        exit(EXIT_FAILURE);
    }

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "internal ring buffer initialized");

    pthread_cleanup_push(cleanupHandler, internal_ring_buffer);

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Registered cleanup handler");

    masterStartupDialogue();

    #ifdef UART_ENABLED

        if(!beginUART()) pthread_exit(NULL);;

    #endif

    (void)printf("Entering main loop\n");

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Entering main loop");

    //TESTME : this whole loop logic needs THOROUGH testing

    while(true){

        pthread_testcancel();


        #ifdef UART_ENABLED

            num_data_points = getUARTData(channel_data_point);

            // plot_point(channel_data_point[0], threshold);

        #else

            num_data_points = getMockUARTData(channel_data_point);

        #endif

        #ifdef ASSERT_ENABLED

            assert(num_data_points % NUM_CHANNELS == 0);

        #endif

        if(num_data_points > 0){

            data_intake_count += num_data_points;

            for(i = 0; i < num_data_points; i++){

                addFloatToRingBuffer(internal_ring_buffer, channel_data_point[i]);

            }//printf("\n");

            if(data_intake_count % WINDOW_SIZE == 0){
                // printf("window size found in data\n");
                
                const int signal_length = SAMPLING_RATE * TIME_IN_WINDOW;

                size_t event_count[NUM_CHANNELS] = {0};

                float channel_windows[NUM_CHANNELS][(size_t)(SAMPLING_RATE * TIME_IN_WINDOW)];

                data_intake_count = 0;

                const size_t extract_index = internal_ring_buffer->write;
             
                extractBufferFromRingBuffer(internal_ring_buffer, window_buffer, WINDOW_SIZE, minusTail(extract_index, WINDOW_SIZE), extract_index);

                for(int i = 0; i < NUM_CHANNELS; i++){
                    for(int j = 0; j < signal_length; j++){

                        const int idx = (j * NUM_CHANNELS) + i;
                        channel_windows[i][j] = window_buffer[idx]; 
                    }

                    event_count[i] = adaptiveThreshold(//event counts have events separated by 1 index
                        channel_windows[i],
                        signal_length,
                        SAMPLING_RATE,
                        TIME_IN_WINDOW,
                        blink_indices,
                        THRESH_MULT
                    ); 
                }
                
                // indentify where in the window the blink is positioned
                // no divide by 2 because of channel 2 shift
                // const size_t blink_in_window_start = NUM_SAMPLES_IN_BLINK;
                const size_t blink_in_window_end = WINDOW_SIZE - (size_t)(NUM_SAMPLES_IN_BLINK);
                
                for(i = 0; i < NUM_CHANNELS; i++){

                    for(size_t j = 0; j < event_count[i]; j++){
                        
                        const size_t event_index = (internal_ring_buffer->write + (blink_indices[j] * NUM_CHANNELS) + i) % internal_ring_buffer->size;
                        
                        
                        // printf("%zu <= %zu <= %zu\n", blink_in_window_start, blink_indices[j], blink_in_window_end);

                        // printf(" channel %d idx %d : %d\n",i, j, blink_indices[j]);
                        if(blink_indices[j] > blink_in_window_end) continue; //case 3

                        start_event = (internal_ring_buffer->size + event_index - NUM_SAMPLES_IN_BLINK) % internal_ring_buffer->size;

                        end_event = (event_index + NUM_SAMPLES_IN_BLINK) % internal_ring_buffer->size;


                        const size_t size_temp = numElementsBetweenIndexes(internal_ring_buffer->size, start_event, end_event);

                        // printf("start : %d, end : %d, size : %d\n", start_event, end_event, size_temp);

                        float * event_temp = malloc(size_temp * sizeof(float));

                        if(!event_temp) continue;

                        extractBufferFromRingBuffer(internal_ring_buffer, event_temp, size_temp, start_event, end_event);

                        const size_t size_event = (size_temp / NUM_CHANNELS);

                        float * event = malloc(size_event * sizeof(float));

                        if(!event){
                            free(event_temp);
                            continue;
                        }

                        for(size_t k = 0; k < size_event; k++){

                            event[k] = event_temp[(k * NUM_CHANNELS) + i];

                        }


                        free(event_temp);


                        // printf("SEND EVENT : %d\n", size_event);
                        addEvent(event, size_event);


                        free(event);
                    
                    }
                }
            }            
        }
    }

    pthread_cleanup_pop(1);
}






void * launchDataIntake(void * _){
    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "launching data intake execution");
    
    (void)_; // compilation warning : unused parameter

    dataIntake();

    pthread_exit(NULL);
}
