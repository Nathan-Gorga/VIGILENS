#include "uart.h"



int main(void){
//	printf("init UART\n");
//	printf("char : %c, digit : %d\n", START_BYTE, START_BYTE);
	if(!beginUART()){
		printf("failed to init uart\n");
		return 1;
	}


	if(!sendUARTSignal(START_STREAM)) return 1;

	float data[PACKET_BUFFER_SIZE * NUM_CHANNELS];

	int loop = 20;

	while(--loop){

		
		size_t count = getUARTData(data);
		for(int i = 0; i < count; ++i){
			for(int j = i; j < i * NUM_CHANNELS; ++j){
				printf("%f ", data[j]);
			}printf("\n");
		}
//		if(count > 0){
//			printf("found %d packets : \n", count);
//			for(int i = 0; i < count; i++){
//				for(int j = 0; j < 33; j++){
//					printf("%d ",data[j + 33 * i]);
//				}
//				printf("\n");
///			}
//		}

//		printf("FOUND %d PACKETS\n", count);

		usleep(1000);
	}

	if(!sendUARTSignal(STOP_STREAM)) return 1;

	endUART();

	return 0;


}



