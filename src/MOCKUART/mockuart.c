#include "mockuart.h"

// this function was useful when UART wasn't implemented. 
// feel free to make it send more realistic data for testing
size_t getMockUARTData(float data_points[PACKET_BUFFER_SIZE]){

    const int probability = 1000000; 

    const bool send_signal = 0 == (rand() % probability);

    const size_t size = 124;

    if(send_signal){

        for(size_t i = 0; i < size; i++){

            data_points[i] = 100.0f;

        }

        return size;
    }

    for(size_t i = 0; i < size; i++){

        data_points[i] = 0.0f;

    }

    return size;
}



