
#include "uart.h"



int main(void){

	if(!beginUART()) return 1;
		
	if(!sendUARTSignal(START_STREAM)) return 1;
	
	endUART();
	return 0;
}
