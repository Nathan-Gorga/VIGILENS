#include "dataintake.h"

#define printf(...) printf(DATA_INTAKE_TEXT_COLOR"DATA INTAKE:%d - ",__LINE__); printf(__VA_ARGS__); printf(RESET)

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



void * launchDataIntake(void * arg){
    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "launching data intake execution");
    
    dataIntake();

    pthread_exit(NULL);
}

static void dataIntake(void){//TESTME : test everything
    
    int data_intake_count = 0;
    
    int blink_indices[20];//TESTME : try different sizes

    float threshold = 0.0f;

    struct ring_buffer * internal_ring_buffer;

    size_t i, tail = 0, num_potential_events = 0, num_data_points = 0, writePlusX, size_of_potential_events[50] = {0}, tail_min = 0, tail_max = 0;

    bool freeze_tail = false, is_not_baseline = false, tail_is_overlap = false, loop = false;

    float linear_buffer[INTERNAL_RING_BUFFER_SIZE] = {0.0f}, potential_events[50][ MAX_EVENT_DURATION ] = {0.0f}, channel_data_point[PACKET_BUFFER_SIZE] = {0.0f};

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
PRINTF_DEBUG
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
                // printf("%d : %f | ", i + 1, channel_data_point[i]);
            }//printf("\n");

            if(data_intake_count % WINDOW_SIZE == 0){
                // printf("window size found in data\n");
                
                const int signal_length = SAMPLING_RATE * TIME_IN_WINDOW;

                int event_count[NUM_CHANNELS] = {0};

                float channel_windows[NUM_CHANNELS][(size_t)(SAMPLING_RATE * TIME_IN_WINDOW)];
                PRINTF_DEBUG

                data_intake_count = 0;

                const size_t extract_index = internal_ring_buffer->write;

                // printf("start idx : %d, stop idx : %d\n", extract_index, minusTail(extract_index, WINDOW_SIZE) );

                extractBufferFromRingBuffer(internal_ring_buffer, window_buffer, WINDOW_SIZE, minusTail(extract_index, WINDOW_SIZE), extract_index);


                // printf("signal length : %d\n",signal_length);

                for(int i = 0; i < NUM_CHANNELS; i++){
                    for(int j = 0; j < signal_length; j++){

                        const int idx = (j * NUM_CHANNELS) + i;
                        channel_windows[i][j] = window_buffer[idx]; 
                    }

                    	// for(int j = 0; j < 10; j++){
                        //     printf("sample[%d][%d] : %f\n", i, j, channel_windows[i][j]);
                        // }


                    event_count[i] = adaptiveThreshold(
                        channel_windows[i],
                        signal_length,
                        SAMPLING_RATE,
                        TIME_IN_WINDOW,
                        blink_indices,
                        THRESH_MULT,
                        &threshold
                    ); 
                }


                // for(int i = 0; i < NUM_CHANNELS -1 ; i++){
                 
                //     printf("event count for channel %d : %d\n", i+1, event_count[i]);
                // }
            
            }

                     
            // printf("end of if\n");
        }

        // printf("end of loop\n");

    }

    pthread_cleanup_pop(1);
}

