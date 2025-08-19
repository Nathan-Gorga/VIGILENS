#include "graphing.h"


static void _plot_point(float value, float min, float max, int width){

	if (value < min) value = min;
	if (value > max) value = max;

	int pos = (int)((value - min) / (max - min) * (width - 1));

	for(int i = 0; i < pos; i++) putchar(' ');

	putchar('*');
	putchar('\n');
}


void plot_point(const float point){

	const int timeout = 100;

	static int function_call_count = 0;

	static  float maximum = 0;

	if(function_call_count < timeout){
		++function_call_count;
		return;
	}


	if(point > maximum) maximum = point;

	function_call_count = 0;


	static float t = 0.0f;
	const float dt = 0.02f;

	printf("max : %d", maximum);

        _plot_point(point, -BOUNDARY, BOUNDARY, 100);

	t += dt;

//	usleep(20000);

}
