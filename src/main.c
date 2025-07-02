#include "main.h"
#include "RINGBUFFER/TEST/unittestringbuffer.h"
#include "EVENTDATASTRUCTURES/eventdatastructure.h"


int main(void){
   
    (void)printf("Hello world\n");

    test();





    //TODO : init event ring buffer

    //TODO : launch data intake
    pthread_t data_intake_thread;

    if(pthread_create(&data_intake_thread, NULL, launchDataIntake, NULL) != 0) return 1;

    (void)pthread_join(data_intake_thread, NULL);

    //TODO : wait for ready signal from data intake

    //TODO : wait for keyboard interupt to cancel data intake thread

    //TODO : cancel data intake

    //TODO : free event ring buffer

    return 0;
}

