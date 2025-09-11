#ifndef FILTER_H
#define FILTER_H

#include "../../globaldefinition.h"

typedef struct {
	float b0, b1, b2;
	float a1, a2;
	float z1, z2;
} iir_filter;

void iirFilterInit(iir_filter * filt, const float b0, const float b1, const float b2, const float a1, const float a2);

float iirFilterProcess(iir_filter * filt, const float x);

void setupFilter(void);

float filterDataPoint(const float point);

#endif

