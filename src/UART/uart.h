/*
HEAVY INSPIRATION FROM : https://gist.github.com/ultramcu/cbdc598bbc1fcd0e5f8a

courtesy to https://github.com/ultramcu
*/

#ifndef UART_H
#define UART_H

#include "../globaldefinition.h"
#include "../EVENTDATASTRUCTURES/eventdatastructure.h"
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>

#define UART_ENABLED

#define RASPBERRY_TX_PIN 14
#define RASPBERRY_RX_PIN 15

#define SERIAL_DEVICE "/dev/serial0" //TESTME : this should be the correct serial link, test it just in case
#define UART_BAUDRATE 115200

#define UART_BUFFER_SIZE (size_t)(sizeof(openbci_packet) * SAMPLING_RATE / 2)

#define PACKET_BUFFER_SIZE (size_t)(UART_BUFFER_SIZE / sizeof(openbci_packet))

#define GAIN 1 //TRY : different value for best results (1 - 24)

#define SCALE_FACTOR (double)(4.5f / GAIN / (pow(2.0f, 23.0f) - 1.0f))

#define MACHINE_USEABLE_SCALE_FACTOR 22.2222222222222222222222222222222222222222222222222222222222222222222222222222222222222

#define SAMPLE_TIME_uS (double)(1000000.0f / SAMPLING_RATE)

//makes it more understandable
#define byte uint8_t

#define START_BYTE (byte)0xA0

#define STOP_BYTE (byte)0xC2 //this stop byte code is relevant for user defined AUX Bytes


//https://docs.openbci.com/Cyton/CytonDataFormat/
typedef union{

    byte packet[33];

    struct{

        //HEADER
        byte start_byte; //0x0A
        byte sample_number;

        //DATA (values are 24-bit signed, MSB first)
        byte channel_data[8][3];

        //AUX DATA
        byte aux_data[6];

        //FOOTER
        byte stop_byte; //0xCX where X is 0-F in hex

        /*
        Stop Byte	Byte 27	Byte 28	Byte 29	Byte 30	Byte 31	Byte 32	Name
        0xC0	    AX1	    AX0	    AY1	    AY0	    AZ1	    AZ0	    Standard with accel
        0xC1	    UDF	    UDF	    UDF	    UDF	    UDF	    UDF	    Standard with raw aux
        0xC2	    UDF	    UDF	    UDF	    UDF	    UDF	    UDF	    User defined
        0xC3	    AC	    AV	    T3	    T2	    T1	    T0	    Time stamped set with accel
        0xC4	    AC	    AV	    T3	    T2	    T1	    T0	    Time stamped with accel
        0xC5	    UDF	    UDF	    T3	    T2	    T1	    T0	    Time stamped set with raw aux
        0xC6	    UDF	    UDF	    T3	    T2	    T1	    T0	    Time stamped with raw aux
        */

    }fields;

}openbci_packet;

static_assert(sizeof(openbci_packet) == 33, "openbci_packet size is not 33 bytes");

static_assert(sizeof(((openbci_packet*)0)->fields) == 33, "fields size is not 33 bytes");

enum TX_SIGNAL_TYPE{
    START_STREAM = 'b',
    STOP_STREAM = 's', 
    NUM_SIGNAL_TYPES
};



bool beginUART(void);//TODO : write function comment

/**
 * @brief Close the serial file descriptor.
 */void endUART(void);


size_t getUARTData(float data_points[PACKET_BUFFER_SIZE]); //TODO : write function comment

bool sendUARTSignal(const enum TX_SIGNAL_TYPE signal_type);//TODO : write function comment

static u_int32_t UART_fd;

#endif

