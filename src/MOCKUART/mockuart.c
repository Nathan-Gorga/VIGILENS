#include "mockuart.h"


size_t getMockUARTData(float data_points[PACKET_BUFFER_SIZE]){//this function needs to return 0s

    const int probability = 1000000; 

    const bool send_signal = 0 == (rand() % probability);

    const size_t size = 124;

    if(send_signal){
        printf(CYAN"SENDING SIGNAL\n"RESET);
        for(int i = 0; i < size; i++){

            data_points[i] = 100.0f;

        }

        return size;
    }

    for(int i = 0; i < size; i++){

        data_points[i] = 0.0f;

    }

    return size;
}



