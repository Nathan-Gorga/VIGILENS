#include "filter.h"

iir_filter hp_filter;
iir_filter lp_filter;

// this wrapper is so we can call this function externally without having to worry 
// about the local scope of the filter
float filterDataPoint(const float point){

	const float y = iirFilterProcess(&hp_filter, point);

	return iirFilterProcess(&lp_filter, y);

}

void iirFilterInit(
    iir_filter * filt, 
    const float b0, 
    const float b1, 
    const float b2, 
    const float a1, 
    const float a2
){

	filt->b0 = b0;
	filt->b1 = b1;
	filt->b2 = b2;
	filt->a1 = a1;
	filt->a2 = a2;
	filt->z1 = 0.0f;
	filt->z2 = 0.0f;

}

float iirFilterProcess(iir_filter * filt, const float x){

	const float y = filt->b0 * x + filt->z1;

	filt->z1 = filt->b1 * x - filt->a1 * y + filt->z2;

	filt->z2 = filt->b2 * x - filt->a2 * y;

	return y;
}

void setupFilter(void){

    //constants for filter (0.5-10Hz @ 250Hz) calculated before hand
	iirFilterInit(
        &hp_filter,
        0.98238544f,
        -1.96477088f, 
        0.98238544f,
		-1.96446058f, 
        0.96508117f);

	iirFilterInit(
        &lp_filter,
	    0.01335920f, 
        0.02671840f, 
        0.01335920f,
		-1.64745998f, 
        0.70089678f);

}



