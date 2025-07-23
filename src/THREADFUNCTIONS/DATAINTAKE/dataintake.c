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
    
    (void)printf("Thread Ready!\n");

    MUTEX_UNLOCK(&ready_lock);

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Sent ready signal to master");

    //wait for go signal
    if(sigwait(&set, &sig) == 0) {
        
        (void)printf("Received SIGCONT, continuing execution.\n");

        (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Received SIGCONT, continuing execution.");
        
    }else {

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

    printf("data intake size : %d\n", INTERNAL_RING_BUFFER_SIZE);

    static int intake_count = 0;

    struct ring_buffer * internal_ring_buffer;

    size_t i, tail = 0, num_potential_events = 0, num_data_points = 0, writePlusX, size_of_potential_events[50] = {0}, tail_min = 0, tail_max = 0;

    bool freeze_tail = false, is_not_baseline = false, tail_is_overlap = false, loop = false;

    float linear_buffer[INTERNAL_RING_BUFFER_SIZE] = {0.0f}, potential_events[50][ MAX_EVENT_DURATION ] = {0.0f}, channel_data_point[PACKET_BUFFER_SIZE] = {0.0f};

    (void)printf("Thread launched succesfully\n");
    
    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "thread launch successfully");

    internal_ring_buffer = initRingBuffer(INTERNAL_RING_BUFFER_SIZE, INTERNAL_RING_BUFFER);
    
    if(internal_ring_buffer == NULL) {

        (void)logEntry(THREAD_DATA_INTAKE, LOG_ERROR , "failure initializing internal ring buffer");
        
        exit(EXIT_FAILURE);
        
    }

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "internal ring buffer initialized");
    
    pthread_cleanup_push(cleanupHandler, internal_ring_buffer);

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Registered cleanup handler");

    masterStartupDialogue();

    #ifdef UART_ENABLED

        char maximum_tries = 10;

        while(!sendUARTSignal(START_STREAM) && --maximum_tries) usleep(10 * 1000);

        if(maximum_tries <= 0) {
            
            (void)logEntry(THREAD_DATA_INTAKE, LOG_ERROR, "UART failed to send start stream signal");
            
            (void)printf("Failed to send start stream signal\n");
            
            pthread_exit(NULL);
        }

    #endif

    (void)printf("Entering main loop\n");

    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "Entering main loop");

    //TESTME : this whole loop logic needs THOROUGH testing

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

        if(num_data_points > 0){

//            printf("%d/%d : %f\%\n", intake_count,INTERNAL_RING_BUFFER_SIZE, ((float)intake_count/(float)INTERNAL_RING_BUFFER_SIZE) * 100);

	    intake_count += num_data_points;

            if(loop && ((!tail_is_overlap && tail_min < internal_ring_buffer->write && internal_ring_buffer->write < tail_max) ||  // if there is no overlap over the point 0, then we check if it is in the interval
                        (tail_is_overlap && (tail_min < internal_ring_buffer->write || internal_ring_buffer->write < tail_max)))){ // if there is an overlap, then we just need one of the two conditions to be truw

		printf("one loop\n");

		intake_count = 0;

                (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "The internal ring buffer has completed a loop since first signal, checking for events...");

                extractBufferFromRingBuffer(internal_ring_buffer, linear_buffer, INTERNAL_RING_BUFFER_SIZE, tail, minusTail(tail, 1));

                num_potential_events = markEventsInBuffer(linear_buffer, INTERNAL_RING_BUFFER_SIZE, potential_events, size_of_potential_events);

                char message[255];

                (void)sprintf(message,"%d events found", num_potential_events);

		printf(message);


                (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, message);

                for(int i = 0; i < num_potential_events; i++){

                    addEvent(potential_events[i], size_of_potential_events[i]);

                }

                (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "added events to event buffer");

                freeze_tail = false;

                loop = false;

            } else if(!loop){

                is_not_baseline = false;

                for(i = 0; i < NUM_CHANNELS; i++){ //TODO : change the baseline parameters to fit the output of the openBCI function

                    is_not_baseline |= !isBaseline(channel_data_point[i], THRESHOLD_MAX, THRESHOLD_MIN);

                }

                if(is_not_baseline){

                    freeze_tail = true;
                    
                    tail_min = minusTail(tail, num_data_points);
                    
                    tail_max = addTail(tail, num_data_points);

                    loop = true;

                }

                tail_is_overlap = tail_min > tail_max;

            }

            for(i = 0; i < num_data_points; i++){

                addFloatToRingBuffer(internal_ring_buffer, channel_data_point[i]);

            }

            tail = !freeze_tail ? internal_ring_buffer->write : tail;

        }
    }

    pthread_cleanup_pop(1);
}

