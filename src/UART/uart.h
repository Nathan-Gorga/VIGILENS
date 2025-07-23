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

#define UART_ENABLED

#define RASPBERRY_TX_PIN 14
#define RASPBERRY_RX_PIN 15

#define SERIAL_DEVICE "/dev/serial0" //TESTME : this should be the correct serial link, test it just in case
#define UART_BAUDRATE 115200

#define UART_BUFFER_SIZE (size_t)(sizeof(openbci_packet) * SAMPLING_RATE / 2)

#define PACKET_BUFFER_SIZE (size_t)(UART_BUFFER_SIZE / sizeof(openbci_packet))

#define GAIN 1 //TRY : different value for best results (1 - 24)
#define SCALE_FACTOR (double)(4.5f / GAIN / (pow(2.0f, 23.0f) - 1.0f))

#define SAMPLE_TIME_uS (double)(1000000.0f / SAMPLING_RATE)

//makes it more understandable
#define byte unsigned char

#define START_BYTE (byte)0x0A

#define STOP_BYTE_0 (byte)0xC0
#define STOP_BYTE_1 (byte)0xC1
#define STOP_BYTE_2 (byte)0xC2
#define STOP_BYTE_3 (byte)0xC3
#define STOP_BYTE_4 (byte)0xC4
#define STOP_BYTE_5 (byte)0xC5
#define STOP_BYTE_6 (byte)0xC6



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

/**
 * @brief Interpret 3 bytes as a signed 24-bit integer. (-8,388,608 to +8,388,607)
 *
 * @param data An array of 3 bytes to be interpreted as a signed 24-bit integer.
 *
 * @return The signed 24-bit integer represented by the 3 bytes.
 *
 * @details This function takes in 3 bytes and interprets them as a signed 24-bit integer.
 *          The bytes are arranged in MSB first.
 */static int32_t interpret24BitToInt(const byte data[3]);


static inline float convertToFloat(const int32_t value);


/**
 * @brief Converts 3 bytes of raw channel data to a usable float.
 *
 * @param data An array of 3 bytes to be interpreted as a signed 24-bit integer.
 *
 * @return The float represented by the 3 bytes.
 *
 * @details This function takes in 3 bytes and interprets them as a signed 24-bit integer.
 *          The bytes are arranged in MSB first.
 */static inline float channelDataToFloat(const byte data[3]);

static bool openSerialFileDescriptor(void);

static bool setTermiosOptions(void);//TODO : write function comment


/**
 * @brief Extracts channel data from an OpenBCI packet and converts it to floats.
 *
 * @param packet The OpenBCI packet containing channel data.
 * @param data_point An array to store the converted float values for each channel.
 *
 * @details This function iterates over the channel data in the provided OpenBCI packet,
 *          converts each 3-byte channel data to a float using channelDataToFloat,
 *          and stores the result in the provided data_point array.
 */static void getChannelDataFromPacket(const openbci_packet packet, float data_point[NUM_CHANNELS]);

static int32_t convertBaudrate(const int32_t baudrate);


/**
 * @brief Extracts OpenBCI packets from a UART buffer.
 *
 * @param buffer The buffer containing raw byte data from the UART.
 * @param size_read The number of bytes read into the buffer.
 * @param packets An array to store the extracted OpenBCI packets.
 *
 * @return The number of packets successfully extracted from the buffer.
 *
 * @details This function iterates over the provided buffer, identifying packet start bytes,
 *          and copies complete packets into the provided packets array. The function returns
 *          the number of packets extracted. It assumes that each packet starts with a predefined
 *          START_BYTE and has a fixed size defined by the openbci_packet structure.
 */static size_t getPacketsFromUARTBuffer(const byte buffer[], const size_t size_read, openbci_packet packets[]);




bool beginUART(void);//TODO : write function comment

/**
 * @brief Close the serial file descriptor.
 */void endUART(void);


size_t getUARTData(float data_points[PACKET_BUFFER_SIZE]); //TODO : write function comment

bool sendUARTSignal(const enum TX_SIGNAL_TYPE signal_type);//TODO : write function comment

static u_int32_t UART_fd;

#endif

