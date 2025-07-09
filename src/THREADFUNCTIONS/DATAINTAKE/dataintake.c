#include "dataintake.h"

#define printf(...) printf(DATA_INTAKE_TEXT_COLOR"DATA INTAKE:%d - ",__LINE__); printf(__VA_ARGS__); printf(RESET)


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
    PRINTF_DEBUG
    
    (void)printf("Cancel signal received\n");    
    PRINTF_DEBUG

    if(internal_ring_buffer != NULL) freeRingBuffer((struct ring_buffer *)internal_ring_buffer);
    PRINTF_DEBUG

    (void)printf("Cleaned up thread\n");   
    PRINTF_DEBUG
    
}

void * launchDataIntake(void * arg){
    
    dataIntake();

    pthread_exit(NULL);
}

static void dataIntake(void){//TESTME : test everything
    //CLEANME
    
    char START_STREAM = 'b'; //TODO : remove

    (void)printf("Thread launched succesfully\n");

    struct ring_buffer * internal_ring_buffer = initRingBuffer(INTERNAL_RING_BUFFER_SIZE, INTERNAL_RING_BUFFER);
    
    if(internal_ring_buffer == NULL) exit(EXIT_FAILURE);
    PRINTF_DEBUG
    
    pthread_cleanup_push(cleanupHandler, internal_ring_buffer);
    PRINTF_DEBUG

    masterStartupDialogue();



    size_t tail = 0;
    bool freeze_tail = false;
    
    float linear_buffer[INTERNAL_RING_BUFFER_SIZE] = {0.0f};
    
    size_t num_potential_events = 0;
    size_t size_of_potential_events[50] = {0};
    float potential_events[50][ MAX_EVENT_DURATION ] = {0.0f};
    

    size_t num_data_points = 0;
    float channel_data_point[PACKET_BUFFER_SIZE] = {0.0f};


    (void)printf("Entering main loop\n");

    {
        char maximum_tries = 10;

        // while(!sendUARTSignal(START_STREAM) && maximum_tries-- ) usleep(10 * 1000); TODO : uncomment 

        if(maximum_tries <= 0) {
            (void)printf("Failed to send start stream signal\n");
            pthread_exit(NULL);
        }
    }   

    //TESTME : this whole loop logic needs THOROUGH testing

    bool send_signal = false;
    const float baseline = 0.0f;
    const float signal = 100.0f;

    unsigned long long counter = 0;

    while(true){

        pthread_testcancel();

        num_data_points = 2; 
        
        if(send_signal){

            channel_data_point[0] = signal; channel_data_point[1] = signal; //TODO : uncomment (getUARTData(channel_data_point);)

        } else {

            channel_data_point[0] = baseline; channel_data_point[1] = baseline; //TODO : uncomment (getUARTData(channel_data_point);)

        }

        assert(num_data_points % NUM_CHANNELS == 0);

        if(num_data_points > 0){

            size_t writePlusOne = writeIndexAfterAddingX(internal_ring_buffer, num_data_points); 
            
            if(tail == writePlusOne){

                PRINTF_DEBUG

                extractBufferFromRingBuffer(internal_ring_buffer, linear_buffer, INTERNAL_RING_BUFFER_SIZE, tail, internal_ring_buffer->write);

                num_potential_events = markEventsInBuffer(linear_buffer, INTERNAL_RING_BUFFER_SIZE, potential_events, size_of_potential_events);//FIXME : diverging from the size of the expected buffers slightly often deals in undefined behavior, find a solution to make this more robust

                for(int i = 0; i < num_potential_events; i++){

                    addEvent(potential_events[i], size_of_potential_events[i]);

                }

                

                freeze_tail = false;

            } else {

                

                bool is_not_baseline = false;

                for(int i = 0; i < NUM_CHANNELS; i++){ 

                    is_not_baseline |= !isBaseline(channel_data_point[i], THRESHOLD_MAX, THRESHOLD_MIN); 

                }

                if(is_not_baseline) freeze_tail = true;
                
            }

            for(int i = 0; i < num_data_points; i++){
                    
                addFloatToRingBuffer(internal_ring_buffer, channel_data_point[i]);

            }

            tail = !freeze_tail ? internal_ring_buffer->write : tail;
        }


        counter++;
        if(counter % 100 == 0) send_signal = !send_signal;
        
    }

    pthread_cleanup_pop(1);
}

