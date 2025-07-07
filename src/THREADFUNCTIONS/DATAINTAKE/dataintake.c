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
    
    (void)printf("Thread Ready!\n");

    MUTEX_UNLOCK(&ready_lock);


    //wait for go signal
    if(sigwait(&set, &sig) == 0) {
        (void)printf("Received SIGCONT, continuing execution.\n");
        
    }else {
        (void)printf("Error waiting for go signal\n");
        pthread_exit(NULL);
    }
}

static void cleanupHandler(void * internal_ring_buffer){
    
    (void)printf("Cancel signal received\n");    

    freeRingBuffer((struct ring_buffer *)internal_ring_buffer);

    (void)printf("Cleaned up thread\n");   
    
}

void * launchDataIntake(void * arg){
    
    dataIntake();

    pthread_exit(NULL);
}

static void dataIntake(void){//TESTME : test everything
    //CLEANME
    
    (void)printf("Thread launched succesfully\n");

    struct ring_buffer * internal_ring_buffer = initRingBuffer(INTERNAL_RING_BUFFER_SIZE, INTERNAL_RING_BUFFER);
    
    if(internal_ring_buffer == NULL) exit(EXIT_FAILURE);
    
    pthread_cleanup_push(cleanupHandler, internal_ring_buffer);

    masterStartupDialogue();



    size_t tail = 0;
    bool freeze_tail = false;
    
    float linear_buffer[INTERNAL_RING_BUFFER_SIZE] = {0.0f};
    
    size_t num_potential_events = 0;
    size_t size_of_potential_events[(size_t)( INTERNAL_RING_BUFFER_SIZE / MAX_EVENT_SIZE )] = {0};
    float potential_events[(size_t)( INTERNAL_RING_BUFFER_SIZE / MAX_EVENT_SIZE )][ MAX_EVENT_SIZE ] = {0.0f};
    
    const float arbitrary_max = 10.0f, arbitrary_min = -10.0f;
    

    size_t num_data_points = 0;
    float channel_data_point[PACKET_BUFFER_SIZE] = {0.0f};


    (void)printf("Entering main loop\n");

    {
        short maximum_tries = 10;

        while(!sendUARTSignal(START_STREAM) && maximum_tries-- ) usleep(10 * 1000);

        if(maximum_tries <= 0) {
            (void)printf("Failed to send start stream signal\n");
            pthread_exit(NULL);
        }
    }   

    //TESTME : this whole loop logic needs THOROUGH testing
    while(true){

        pthread_testcancel();

        num_data_points = getUARTData(channel_data_point);

        assert(num_data_points % NUM_CHANNELS == 0);

        if(num_data_points > 0){

            const size_t writePlusOne = writeIndexAfterIncrement(internal_ring_buffer);

            if(tail == writePlusOne){

                extractBufferFromRingBuffer(internal_ring_buffer, linear_buffer, INTERNAL_RING_BUFFER_SIZE, tail, internal_ring_buffer->write);

                //TODO : get events from extracted buffer (returns number of potential events, potential_events is filled with that number of events as well as the size of each events in the above array)

                for(int i = 0; i < num_potential_events; i++){

                    addEvent(potential_events[i], size_of_potential_events[i]);

                }

                for(int i = 0; i < num_data_points; i++){
                    
                    addFloatToRingBuffer(internal_ring_buffer, channel_data_point[i]);

                }

                freeze_tail = false;

            } else {

                for(int i = 0; i < num_data_points; i++){
                    
                    addFloatToRingBuffer(internal_ring_buffer, channel_data_point[i]);

                }
                
                bool is_not_baseline = false;

                for(int i = 0; i < NUM_CHANNELS; i++){ 

                    is_not_baseline |= !isBaseline(channel_data_point[i], arbitrary_max, arbitrary_min); //TESTME

                }

                if(is_not_baseline){
                    
                    freeze_tail = true;

                }
            }
        
            tail = !freeze_tail ? internal_ring_buffer->write : tail;
        }
    }

    pthread_cleanup_pop(1);
}

