#ifndef UART_H
#define UART_H

#include "../globaldefinition.h"
#include "../EVENTDATASTRUCTURES/eventdatastructure.h"
#include <fcntl.h>
#include <termios.h>

#define RASPBERRY_TX_PIN 14
#define RASPBERRY_RX_PIN 15

#define GAIN 24 //default
#define SCALE_FACTOR (double)(4.5f / GAIN / (pow(2.0f, 23.0f) - 1.0f))

//makes it more understandable
#define byte char 

//https://docs.openbci.com/Cyton/CytonDataFormat/
typedef union openbci_packet{

    byte packet[33];

    //HEADER
    byte start_byte; //0x0A
    byte sample_number;

    //DATA (values are 24-bit signed, MSB first)
    byte channel_1[3];
    byte channel_2[3];
    byte channel_3[3];
    byte channel_4[3];
    byte channel_5[3];
    byte channel_6[3];
    byte channel_7[3];
    byte channel_8[3];

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

}openbci_packet;

enum TX_SIGNAL_TYPE{
    START_STREAM, // FIXME : assign them the value necessary for communicating with openBCI
    STOP_STREAM,
    NUM_SIGNAL_TYPES
};

static int32_t interpret24BitToInt(const byte data[3]);//TODO : write function comment

static inline float convertToFloat(const int32_t value);//TODO : write function comment

static inline float channelDataToFloat(const byte data[3]);//TODO : write function comment

static openSerialPort(void);//TODO : write function comment

void initUart(void);//TODO : write function comment

static closeSerialPort(void);//TODO : write function comment

void closeUart(void);//TODO : write function comment

bool getUartData(float data_point[NUM_CHANNELS]);//TODO : write function comment

static void getChannelDataFromPacket(const openbci_packet packet, float data_point[NUM_CHANNELS]);//TODO : write function comment

void sendUartSignal(enum TX_SIGNAL_TYPE signal_type);//TODO : write function comment

#endif
