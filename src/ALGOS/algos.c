#include "algos.h"


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

size_t markEventsInBuffer(float buffer[], const size_t size_buffer, float **events, size_t size_events[]){//TESTME 
    //TRY : had an alternative to this algo, was faster but made things harder to undertand. went for simplicity.
    //the algo actually first stores the SIZE of each distinct segment, not if every point is baseline or not it then works based on that
    //the complexity comes when you have to workout the real index in the buffer once you found them

    assert(buffer != NULL);

    assert(events != NULL);

    assert(size_buffer > 0);

    assert(size_events != NULL);

    
    bool * is_baseline = malloc(size_buffer * sizeof(bool));
    if(is_baseline == NULL) return -1;


    for(int i = 0; i < size_buffer; i++){
        is_baseline[i] = isBaseline(buffer[i], THRESHOLD_MAX, THRESHOLD_MIN);
    }

    //STEP 2

    size_t start = -1, max = -1, min = -1, counter_potential_events = 0;

    int j;

    for(int i = 0; i < size_buffer; i++){
        
        const bool point_is_baseline = is_baseline[i];

        if(!point_is_baseline){
            //set start-  a bit, min, max 
            start = Max(i - DETECTION_TOLERANCE, 0);
            min = Min(start + MIN_EVENT_DURATION, size_buffer - 1);
            max = Min(start + MAX_EVENT_DURATION, size_buffer - 1);

            // #1 go to the end of that not baseline event
find_end_of_signal_segment:
            
            for(i; is_baseline[i + 1] || i < size_buffer; i++);

            //is th baseline after that less than min baseline?
            j = i + 1;

            for(j; !is_baseline[j + 1] || j < size_buffer; j++);

            if(j - i < MIN_EVENT_DURATION){            
                
                i = Min( j + 1, size_buffer - 1);
                goto find_end_of_signal_segment;//FIXME : use something other than goto
                

            } else {

                if(min < i && i < max){
                        
                    size_events[counter_potential_events] = i - start;

                    for(int k = 0; k < size_events[counter_potential_events]; k++){//TODO : replace with memcpy
                        events[counter_potential_events][k] = buffer[start + k];
                    }

                    counter_potential_events++;
                        
                } else {
                    //NULL event
                    start = -1;
                    min = -1;
                    max = -1;
                }
            }
        }
    }


    free(is_baseline);
    return counter_potential_events;
}




