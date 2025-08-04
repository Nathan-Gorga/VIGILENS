
#include "uart.h"



int main(void){
	printf("test\n");
	if(!beginUART()) return 1;
		
	if(!sendUARTSignal(START_STREAM)) return 1;
	

	float data[PACKET_BUFFER_SIZE];
	int count = 0;

	int n = 0;


	int loop = 1000000;

	while(--loop){

		printf("inside the loop\n");

		memset(data, 0, sizeof(data));

		n = read(UART_fd, data, sizeof(data) - 1);

		if(n < 0){
			perror("Error reading from serial port");
			break;
		} else if(n > 0){
			data[n] = '\0';
			printf("Received %s", data); fflush(stdout);
		}

		printf("loop : %d\n", loop);
		//count = getUARTData(data);
	}


	if(!sendUARTSignal(STOP_STREAM)) return 1;



	endUART();
	return 0;
}
