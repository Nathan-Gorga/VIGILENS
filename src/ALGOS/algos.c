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

size_t markEventsInBuffer(float buffer[], const size_t size_buffer, float events[][MAX_EVENT_DURATION], size_t size_events[]){

    //TESTME : further with artifacts and noise and weird input buffers (like alternating true false for example)
    //FOR FUTURE DEBUGGING/UNDERSTANDING, HERE IS A DIAGRAM OF THE ALGORITHM BELOW : https://www.c  anva.com/design/DAGsqXG_BNU/qtS7Dkq2mF7FSWF6K4jTbQ/edit

    #ifdef ASSERT_ENABLED

    // assert(buffer != NULL);

    // assert(events != NULL);

    // assert(size_buffer > 0);

    // assert(size_events != NULL);

    #endif

    enum {
        SIGNAL = false,
        BASELINE = true
    }TYPE;//makes it clearer
    
    size_t start = -1, Max = -1, Min = -1, counter_potential_events = 0;
    bool investigating = false;
    int i, j;

    bool bool_buffer[INTERNAL_RING_BUFFER_SIZE];// NOTE : for now this is fine because it's the only use for this function but the size may be too small if used on event ring buffer

    if(bool_buffer == NULL) return -1;

    for(i = 0; i < size_buffer; i++){

        bool_buffer[i] = isBaseline(buffer[i], THRESHOLD_MAX, THRESHOLD_MIN);
    }

    for(i = 0; i < size_buffer; i++){

        if(bool_buffer[i] == SIGNAL){

            if(!investigating){
   
                //SET INVESTIGATION VALUES Of A SEGMENT
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
            if(j - i >= MIN_BASELINE_DURATION && j < size_buffer){

                //THE SIGNAL IS NOT TOO LONG OR TOO SHORT
                if(Min < i && i < Max){
                    
                    //MARK IT AS AN EVENT
                    size_events[counter_potential_events] = i - start;

                    (void)memmove(events[counter_potential_events], buffer + start, size_events[counter_potential_events] * sizeof(float));                    
                    
                    counter_potential_events++;     
                }

                investigating = false;

            }
            //NOTE : if things start to break, put the i=j in an else{} statement (might help)
            i = j;
        }
    }

    return counter_potential_events;
}





bool simpleThresholdEventDetection(const float threshold, const float event[MAX_EVENT_DURATION], const size_t size){

	for(int i = 0; i < size; ++i){
		if(threshold <= event[i]) return true;
	}

	return false;
}


iir_filter bp_filter;


float filterDataPoint(const float point){//this wrapper is so we can call this function externally without having to worry about the scope of the filter
	return irrFilterProcess(&bp_filter, point);
}//TESTME



void iirFilterInit(irr_filter * filt, const float b0, const float b1, const float b2, const float a1, const float a2){
	filt->b0 = b0;
	filt->b1 = b1;
	filt->b2 = b2;
	filt->a1 = a1;
	filt->a2 = a2;
	filt->z1 = 0.0f;
	filt->z2 = 0.0f;
}//TESTME


float irrFilterProcess(irr_filter * filt, const float x){
	const float y = filt->b0 * x + filt->z1;
	filt->z1 = filt->b1 * x - filt->a1 * y + filt->z2;
	filt->z2 = filt->b2 * x - filt->a2 * y;
	return y;
}//TESTME

void setupFilter(void){
	iirFilterInit(&bp_filter,
		      0.02008337f, 0.0f, -0.02008337f,
		      -1.959934f, 0.9598333f);//constants for filter (0.5-10Hz @ 250Hz) calculated before hand
}//TESTME
