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

size_t markEventsInBuffer(float buffer[], const size_t size_buffer, float events[][MAX_EVENT_DURATION ], size_t size_events[]){

    //TESTME : further with artifacts and noise and weird input buffers (like alternating true false for example)
    //FOR FUTURE DEBUGGING/UNDERSTANDING, HERE IS A DIAGRAM OF THE ALGORITHM BELOW : https://www.c  anva.com/design/DAGsqXG_BNU/qtS7Dkq2mF7FSWF6K4jTbQ/edit

    assert(buffer != NULL);

    assert(events != NULL);

    assert(size_buffer > 0);

    assert(size_events != NULL);

    enum {
        SIGNAL = false,
        BASELINE = true
    }TYPE;//makes it clearer
    
    size_t start = -1, Max = -1, Min = -1, counter_potential_events = 0;
    bool investigating = false;
    int i, j;

    bool * bool_buffer = malloc(size_buffer * sizeof(bool));//FIXME : avoid the heap, especially in a time sensitive function

    if(bool_buffer == NULL) return -1;

    for(i = 0; i < size_buffer; i++){

        bool_buffer[i] = isBaseline(buffer[i], THRESHOLD_MAX, THRESHOLD_MIN);
    }

    for(i = 0; i < size_buffer; i++){

        if(bool_buffer[i] == SIGNAL){

            if(!investigating){
   
                //SET INVESTIGATION VALUES ON A SEGMENT
                start = max(i - DETECTION_TOLERANCE, 0);
                Min = min(start + MIN_EVENT_DURATION, size_buffer - 1);
                Max = min(start + MAX_EVENT_DURATION, size_buffer - 1);

                investigating = true;
            }

            //GO TO THE END OF THE SIGNAL
            for(i; i < size_buffer && bool_buffer[i + 1] == SIGNAL; i++);

            j = i + 1;

            //GO TO THE END OF THE BASELINE (TO MEASURE IT'S SIZE)
            for(j; bool_buffer[j + 1] == BASELINE && j < i + 1 + MAX_EVENT_DURATION; j++);

            //THE BASELINE IS LONG ENOUGH AND WE ARE NOT OVER THE BUFFER
            if(j - i > MIN_BASELINE_DURATION || j >= size_buffer ){

                //THE SIGNAL IS NOT TOO LONG OR TOO SHORT
                if(Min < i && i < Max){
                    
                    //MARK IT AS AN EVENT
                    size_events[counter_potential_events] = i - start;

                    (void)memmove(events[counter_potential_events], buffer + start, size_events[counter_potential_events] * sizeof(float));

                    counter_potential_events++;     
                }

                investigating = false; 

            } else {//FIXME : this else condition may not be useful at all and i = j should happen no matter what
                //CONTINUE INVESTIGATION AS THE BASELINE WAS TOO SHORT (THE SEGMENT MAY CONTINUES AFTER THIS)
                
                i = j;//i gets increment after the for loop
            }    
        }
    }

    free(bool_buffer);
    return counter_potential_events;
}
