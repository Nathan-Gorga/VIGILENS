#include "graphing.h"


static void _plot_point(float value, float min, float max, int width, const float thresh){

//	putchar('\r');

	if (value < min) value = min;
	if (value > max) value = max;


	int pos = (int)((value - min) / (max - min) * (width - 1));

	const int threshold = (int)((thresh  - min) / (max - min) * (width - 1));

	if(pos < threshold){


		const int diff = threshold - pos;

		for(int i = 0; i < pos; i++) putchar(' ');

		putchar('*');


		for(int i = 0; i < diff; i++) putchar(' ');

		putchar('|');
		putchar('\n');

	} else if (pos > threshold){


		const int diff = pos - threshold;

		for(int i = 0; i < threshold + 1; i++) putchar(' ');

		putchar('|');

		for(int i = 0; i < diff; i++) putchar(' ');


		putchar('*');
		putchar('\n');
	} else {

		for(int i = 0; i < pos + 1; i++) putchar(' ');

		putchar('|');
		putchar('\n');
	}
}

static void plot_threshold(const float threshold){

	const float min = -BOUNDARY;
	const float max = BOUNDARY;

	const int width = 100;

	putchar('\r');

	const int pos = (int)((threshold - min) / (max - min) * (width - 1));

	for(int i = 0; i < pos; i++) printf("\033[1C");//move cursor to the right

	putchar('|');


}


void plot_point(const float point, const float threshold){

	const int timeout = 50;

	static int function_call_count = 0;

	static float maximum = 0;

	if(function_call_count < timeout){

		++function_call_count;

		return;

	}

//	if(point > maximum) maximum = point;

	function_call_count = 0;
	// printf("threshold : %f , point : %f\n", threshold, point);

        _plot_point(point, -BOUNDARY, BOUNDARY, 100, threshold);

//	plot_threshold(threshold);

//	printf("\t\t\tmax : %f", maximum);
}
