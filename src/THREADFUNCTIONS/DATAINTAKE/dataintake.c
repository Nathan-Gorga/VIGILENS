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

void * launchDataIntake(void * arg){
    (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "launching data intake execution");
    
    dataIntake();

    pthread_exit(NULL);
}

static void dataIntake(void){//TESTME : test everything

    struct ring_buffer * internal_ring_buffer;

    size_t i, tail = 0, num_potential_events = 0, num_data_points = 0, writePlusX, size_of_potential_events[50] = {0}, tail_min, tail_max;

    bool freeze_tail = false, is_not_baseline = false, tail_is_overlap = false;

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

        while(!sendUARTSignal(START_STREAM) && maximum_tries--) usleep(10 * 1000);
    
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

            num_data_points = getMockUARTData(channel_data_point);//TODO : test this function and that data intake does it's job PERFECTLY

        #endif

        assert(num_data_points % NUM_CHANNELS == 0);

        if(num_data_points > 0){

            // logEntry(THREAD_DATA_INTAKE, LOG_INFO, "got data from UART"); 
            

            
            if((!tail_is_overlap && tail_min < internal_ring_buffer->write && internal_ring_buffer->write < tail_max) ||// if there is no overlap over the point 0, then we check if it is in the interval
                (tail_is_overlap && (tail_min < internal_ring_buffer->write || internal_ring_buffer->write < tail_max))){ // if there is an overlap, then we just need one of the two conditions to be truw
                printf("test\n");
                printf("tail_is_overlap : %s, tail_min : %zu, write : %zu, tail_max : %zu\n", tail_is_overlap ? "true" : "false", tail_min, internal_ring_buffer->write, tail_max); 

                logEntry(THREAD_DATA_INTAKE, LOG_INFO, "The internal ring buffer has completed a loop since first signal, checking for events...");
                
                extractBufferFromRingBuffer(internal_ring_buffer, linear_buffer, INTERNAL_RING_BUFFER_SIZE, tail, internal_ring_buffer->write);

                num_potential_events = markEventsInBuffer(linear_buffer, INTERNAL_RING_BUFFER_SIZE, potential_events, size_of_potential_events);//FIXME : diverging from the size of the expected buffers slightly often deals in undefined behavior, find a solution to make this more robust

                char message[255];

                sprintf(message,"%d events found\n", num_potential_events);

                logEntry(THREAD_DATA_INTAKE, LOG_INFO, message);


                for(int i = 0; i < num_potential_events; i++){

                    addEvent(potential_events[i], size_of_potential_events[i]);

                }

                (void)logEntry(THREAD_DATA_INTAKE, LOG_INFO, "added events to event buffer");

                freeze_tail = false;

            } else {              


                for(i = 0; i < NUM_CHANNELS; i++){ 

                    is_not_baseline |= !isBaseline(channel_data_point[i], THRESHOLD_MAX, THRESHOLD_MIN); 

                }

                if(is_not_baseline){

                    freeze_tail = true;

                    tail_min = !(tail - num_data_points < 0) ? tail - num_data_points : (tail - num_data_points) + INTERNAL_RING_BUFFER_SIZE;//FIXME : make this in a function

                    tail_max = !(tail + num_data_points > INTERNAL_RING_BUFFER_SIZE) ? tail + num_data_points : (num_data_points + tail) - INTERNAL_RING_BUFFER;//FIXME : make this in a function

                    tail_is_overlap = tail_min > tail_max;

                    // printf("tail : %zu, tail_min, : %zu, tail_max : %zu, tail_is_overlap : %s\n",tail, tail_min, tail_max, tail_is_overlap ? "true" : "false");

                }
            }

            for(i = 0; i < num_data_points; i++){
                    
                addFloatToRingBuffer(internal_ring_buffer, channel_data_point[i]);

            }

            tail = !freeze_tail ? internal_ring_buffer->write : tail;
        }        
    }

    pthread_cleanup_pop(1);
}

