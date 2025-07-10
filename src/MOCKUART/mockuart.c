#include "mockuart.h"


size_t getMockUARTData(float data_points[PACKET_BUFFER_SIZE]){
    
    //TESTME : test this function thoroughly
    
    const int probability = 100; //1% chance

    const bool send_signal = probability - 1 == (rand() % probability);

    if(send_signal){

        for(int i = 0; i < 124; i++){

            data_points[i] = 100.0f;
        }

        return 124;
    }

    return 0;
}







