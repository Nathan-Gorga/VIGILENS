#ifndef UART_H
#define UART_H

#include "../globaldefinition.h"
#include <fcntl.h>
#include <termios.h>


#define RASPBERRY_TX_PIN 14
#define RASPBERRY_RX_PIN 15

#define byte char //makes it more understandable

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





//TODO : open serial port

//TODO : INIT uart

//TODO : close serial port

//TODO : get uart data (takes in as many float pointers as there are channels, and returns a true if there is data else false )

//TODO : unpack openbci packet

//TODO : send uart signal





#endif
