#include "dataintake.h"

static void masterStartupDialogue(void){

    sigset_t set;

    int sig;

    while(sigemptyset(&set));

    while(sigaddset(&set, SIGCONT));

    // Send ready signal to master
    MUTEX_LOCK(&ready_lock);

        ready_count++;

        (void)pthread_cond_signal(&ready_cond);

    MUTEX_UNLOCK(&ready_lock);

    //wait for go signal
    if(sigwait(&set, &sig) == 0) {

        (void)printf(BLUE"INTAKE RECEIVED GO SIGNAL\n"RESET);

    } else {

        (void)printf("Error waiting for go signal\n");

        pthread_exit(NULL);
    }
}

static void cleanupHandler(void * internal_ring_buffer){

    (void)printf("Cancel signal received\n");

    if(internal_ring_buffer != NULL) freeRingBuffer((struct ring_buffer *)internal_ring_buffer);

    (void)printf("Cleaned up thread\n");
}

static void dataIntake(void){
    
    size_t data_intake_count = 0;

    bool missing_data[NUM_CHANNELS] = {false};
    
    size_t blink_indices[20];

    int later_blinks[20];

    int later_counts = 0;

    struct ring_buffer * internal_ring_buffer;

    size_t i, num_data_points = 0;

    // size_t start_event, end_event;

    double channel_data_point[PACKET_BUFFER_SIZE] = {0.0f};

    double window_buffer[WINDOW_SIZE];

    internal_ring_buffer = initRingBuffer(INTERNAL_RING_BUFFER_SIZE, INTERNAL_RING_BUFFER);

    setupFilter();

    if(internal_ring_buffer == NULL) {

        exit(EXIT_FAILURE);
    }

    pthread_cleanup_push(cleanupHandler, internal_ring_buffer);

    masterStartupDialogue();

    #ifdef UART_ENABLED

        if(!beginUART()) pthread_exit(NULL);;

    #endif

    printf(BLUE"INTAKE ENTERING MAIN LOOP\n"RESET);

    while(true){

        pthread_testcancel();

        #ifdef UART_ENABLED

            num_data_points = getUARTData(channel_data_point);

        #else

            num_data_points = getMockUARTData(channel_data_point);

        #endif

        #ifdef ASSERT_ENABLED

            assert(num_data_points % NUM_CHANNELS == 0);

        #endif

        
        if(num_data_points <= 0) continue;

        data_intake_count += num_data_points;

        for(i = 0; i < num_data_points; i++) addFloatToRingBuffer(internal_ring_buffer, channel_data_point[i]);

        if(data_intake_count < WINDOW_SIZE) continue;
        
        const int signal_length = SAMPLING_RATE * TIME_IN_WINDOW;

        size_t event_count[NUM_CHANNELS] = {0};

        double channel_windows[NUM_CHANNELS][(size_t)(SAMPLING_RATE * TIME_IN_WINDOW)];

        data_intake_count = 0;

        const size_t extract_index = internal_ring_buffer->write;
        const int s =  minusTail(extract_index, WINDOW_SIZE);
        

        extractBufferFromRingBuffer(internal_ring_buffer, window_buffer, WINDOW_SIZE,s, extract_index);

        for(int i = 0; i < NUM_CHANNELS; i++){

            for(int j = 0; j < signal_length; j++){

                const int idx = (j * NUM_CHANNELS) + i;

                channel_windows[i][j] = window_buffer[idx]; 

            }

            event_count[i] = adaptiveThreshold(
                channel_windows[i],
                signal_length,
                // SAMPLING_RATE,
                // TIME_IN_WINDOW,
                blink_indices,
                THRESH_MULT,
                &missing_data[i]
            ); 
        
            if(event_count[i] <= 0) continue;


            int to_send_count = 0;
            int to_send[40];

            //get all the blinks in one channel                
            for(size_t j = 0; j < event_count[i] && !missing_data[i]; j++){
            
                printf(BLUE"sending CURRENT blinks\n"RESET);

                //get indexes from blink_indices (absolute to ring buffer) and later buffer
                to_send[to_send_count++] = (internal_ring_buffer->write + (blink_indices[j] * NUM_CHANNELS) + i) % internal_ring_buffer->size;
                                           
            }
            
            //send all the blinks in later_blinks
            while(later_counts > 0){
                printf(BLUE"sending later blinks\n"RESET);

                to_send[to_send_count++] = later_blinks[--later_counts];

            }

            const int B = 250;// TODO : make this a macro constant so you can declare event_buffer on the stack
            const int size_buf = B * 2;

            double * event_buffer = malloc(size_buf * sizeof(double));

            for(int j = 0; j < to_send_count; j++){

                const int event_index = to_send[j];

                const int start_index = (internal_ring_buffer->size + event_index - B) % internal_ring_buffer->size;
                const int end_index = (internal_ring_buffer->size + event_index + B) % internal_ring_buffer->size;

                // extract the whole data
                extractBufferFromRingBuffer(internal_ring_buffer, event_buffer, size_buf, start_index, end_index);

                for(int k = 0; k < size_buf/2; k++){
                    event_buffer[k] = event_buffer[(k * NUM_CHANNELS) + i];
                }

                //send it to processing thread
                addEvent(event_buffer, size_buf / 2);
            }

            free(event_buffer); 

            if(!missing_data[i]) continue;

            // add channel index (absolute to ring buffer to later buffer
            for(size_t j = 0; j < event_count[i]; j++){

                later_blinks[later_counts++] = (internal_ring_buffer->write + (blink_indices[j] * NUM_CHANNELS) + i) % internal_ring_buffer->size;

            }

            // switch the missing data flag
            missing_data[i] = false;
                        
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
