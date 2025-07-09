#include "algos.h"


#ifdef DEBUG_ALGO
    #define PRINTF_DEBUG do{printf(PURPLE"DEBUG : %s - %s:%d\n"RESET, __FILE__, __func__,__LINE__); fflush(stdout);}while(0);
#else
    #define PRINTF_DEBUG
#endif

inline bool aboveThreshold(const float data_point, const float threshold){//DONTTOUCH
    return data_point >= threshold;
}


inline bool belowThreshold(const float data_point, const float threshold){//DONTTOUCH
    return data_point <= threshold;
}


inline bool isBaseline(const float data_point, const float max, const float min){//DONTTOUCH
    /*
    NOT ( TRUE OR TRUE) = FALSE 
    NOT (TRUE OR FALSE) = FALSE
    NOT (FALSE OR TRUE) = FALSE
    NOT (FALSE OR FALSE) = TRUE
    */
    return !(aboveThreshold(data_point, max) || belowThreshold(data_point, min));
}

size_t markEventsInBuffer(float buffer[], const size_t size_buffer, float events[][MAX_EVENT_DURATION * 2], size_t size_events[]){//TESTME 
    //TRY : had an alternative to this algo, was faster but made things harder to undertand. went for simplicity.
    //the algo actually first stores the SIZE of each distinct segment, not if every point is baseline or not it then works based on that
    //the complexity comes when you have to workout the real index in the buffer once you found them

    assert(buffer != NULL);

    assert(events != NULL);

    assert(size_buffer > 0);

    assert(size_events != NULL);

    PRINTF_DEBUG    
    bool * is_baseline = malloc(size_buffer * sizeof(bool));
    if(is_baseline == NULL) return -1;

    PRINTF_DEBUG    

    for(int i = 0; i < size_buffer; i++){
        is_baseline[i] = isBaseline(buffer[i], THRESHOLD_MAX, THRESHOLD_MIN);
    }
    PRINTF_DEBUG    

    //STEP 2

    size_t start = -1, Max = -1, Min = -1, counter_potential_events = 0;

    int j;

    PRINTF_DEBUG    

    for(int i = 0; i < size_buffer; i++){
        
        const bool point_is_baseline = is_baseline[i];

        if(!point_is_baseline){
            //set start-  a bit, min, max 
            start = max(i - DETECTION_TOLERANCE, 0);
            Min = min(start + MIN_EVENT_DURATION, size_buffer - 1);
            Max = min(start + MAX_EVENT_DURATION, size_buffer - 1);

            // #1 go to the end of that not baseline event
find_end_of_signal_segment:
            
            for(i; !is_baseline[i + 1] && i < size_buffer; i++);

            // if(i >= size_buffer) break;

            j = i + 1;

            for(j; is_baseline[j + 1] && j < i + 1 + MAX_EVENT_DURATION; j++);

            printf("j : %zu, i : %zu\n", j, i);
            printf("start : %zu\n", start);
            if(j - i < MIN_BASELINE_DURATION && j < size_buffer ){            
                PRINTF_DEBUG    
                
                i = min( j + 1, size_buffer - 1);
                goto find_end_of_signal_segment;//FIXME : use something other than goto
                

            } else {
                PRINTF_DEBUG    

                if(Min < i && i < Max){
                    PRINTF_DEBUG    
                    
                    size_events[counter_potential_events] = i - start;
                    
                    PRINTF_DEBUG    


                    for(int k = 0; k < size_events[counter_potential_events]; k++){//TODO : replace with memcpy
                        events[counter_potential_events][k] = buffer[start + k];
                    }
                    PRINTF_DEBUG    
                    counter_potential_events++;     
                }
                start = -1;
                Min = -1;
                Max = -1;
            }
        }
    }

    free(is_baseline);
    return counter_potential_events;
}


int main(void){

    float buf[1000] = {100.0f};
    const int size_buf = 1000;

    const int start_signal1 = 0;
    const int stop_signal1 = 100;

    const int start_signal2 = 150;
    const int stop_signal2 = 250;


    const int start_signal3 = 300;
    const int stop_signal3 = 400;

    const int start_signal4 = 450;
    const int stop_signal4 = 550;


    const int start_signal5 = 600;
    const int stop_signal5 = 700;


    // for(int i = start_signal1; i < stop_signal1; i++){
    //     buf[i] = 100.0f;
    // }

    // for(int i = start_signal2; i < stop_signal2; i++){
    //     buf[i] = 200.0f;
    // }

    // for(int i = start_signal3; i < stop_signal3; i++){
    //     buf[i] = 300.0f;
    // }


    // for(int i = start_signal4; i < stop_signal4; i++){
    //     buf[i] = 400.0f;
    // }


    // for(int i = start_signal5; i < stop_signal5; i++){
    //     buf[i] = 500.0f;
    // }




    PRINTF_DEBUG

    float events[10][MAX_EVENT_DURATION * 2];//FIXME : find better size for this buffer
    size_t size_events[10];

    size_t num_events = markEventsInBuffer(buf, size_buf, events, size_events);

    printf("num events = %ld\n", num_events);
    for(int i = 0; i < num_events; i++){
        for(int j = 0; j < size_events[i]; j++){
            printf("%f ", events[i][j]);
        }
        printf("\n");
    }

    // TESTME : one test signal
    // TESTME : two test signal far apart
    // TESTME : two test signal close 
    // TESTME : test signal clipped by start
    // TESTME : test signal clipped by start bigger than max
    // TESTME : test signal clipped by start smaller than min
    // TESTME : test signal clipped by end
    // TESTME : test signal clipped by end bigger than max
    // TESTME : test signal clipped by end smaller than min
    // TESTME : test signal less than min
    // TESTME : test signal more than max
    // TESTME : test maximum signals









    




    // TODO : test no baseline


    return 0;
}

