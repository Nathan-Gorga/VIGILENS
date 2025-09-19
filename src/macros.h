#ifndef MACROS_H
#define MACROS_H

#include <stdint.h>

/// GLOBAL

    // color codes for text
    #define BLACK "\x1B[0;30m"
    #define RED "\x1B[0;31m"
    #define GREEN "\x1B[0;32m"
    #define YELLOW "\x1B[0;33m"
    #define BLUE "\x1B[0;34m"
    #define PURPLE "\x1B[0;35m"
    #define CYAN "\x1B[0;36m"
    #define WHITE "\x1B[0;37m"
    #define RESET "\x1B[0m"

    // macro flags, you can toggle them
    #define ASSERT_ENABLED
    // #define DEBUG


    // use this on a low level OS when hunting for SEGFAULTS
    #ifdef DEBUG
        #define PRINTF_DEBUG do{printf(PURPLE"DEBUG : %s - %s:%d\n"RESET, __FILE__, __func__,__LINE__); fflush(stdout);}while(0);
    #else
        #define PRINTF_DEBUG
    #endif

    // makes life a bit easier
    #define MUTEX_LOCK(mutex) if(pthread_mutex_lock(mutex) != 0){ \
                                    (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__);\
                                    pthread_exit(NULL);\
                            }

    #define MUTEX_UNLOCK(mutex) if(pthread_mutex_unlock(mutex) != 0){ \
                                    (void)printf("ERROR in %s:%d\n : You did something you shouldn't have...\n", __FILE__, __LINE__);\
                                    pthread_exit(NULL);\
                                }

    // always useful
    #define min(a,b) ((a) < (b) ? (a) : (b))
    #define max(a,b) ((a) > (b) ? (a) : (b))

/// TECHNICAL SPECS

    // this is depending on the OpenBCI, usually never changes
    #define SAMPLING_RATE 250
    #define NUM_CHANNELS 2

/// UART 

    // this switch can be toggled to switch between mock and true UART
    #define UART_ENABLED

    #define RASPBERRY_TX_PIN 14
    #define RASPBERRY_RX_PIN 15

    #define SERIAL_DEVICE "/dev/serial0"
    #define UART_BAUDRATE 115200

    #define UART_BUFFER_SIZE (size_t)(sizeof(openbci_packet) * SAMPLING_RATE / NUM_CHANNELS)

    #define PACKET_BUFFER_SIZE (size_t)(UART_BUFFER_SIZE / sizeof(openbci_packet))

    #define GAIN 1 // values can be from 1 to 24 (inversely proportional to SCALE_FACTOR)

    //https://docs.openbci.com/Cyton/CytonDataFormat/#interpreting-the-eeg-data
    #define SCALE_FACTOR (double)(4.5f / GAIN / (pow(2.0f, 23.0f) - 1.0f))

    // simply for ease of use, can be set to 1 if causes problems
    #define MACHINE_USEABLE_SCALE_FACTOR 100000 

    #define SAMPLE_TIME_uS (double)(1000000.0f / SAMPLING_RATE)

    //makes it more understandable
    #define byte uint8_t

    #define START_BYTE (byte)0xA0

    #define STOP_BYTE (byte)0xC2 //this stop byte code is relevant for user defined AUX Bytes

    //https://docs.openbci.com/Cyton/CytonDataFormat/#binary-format
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

    // preferable not to touch these assert, ensure the packet is always the right size at compile time
    static_assert(sizeof(openbci_packet) == 33, "openbci_packet size is not 33 bytes");
    static_assert(sizeof(((openbci_packet*)0)->fields) == 33, "fields size is not 33 bytes");

    enum TX_SIGNAL_TYPE{
        START_STREAM = 'b',
        STOP_STREAM = 's', 
        NUM_SIGNAL_TYPES
    };

/// EVENTDATASTRUCTURES

    #define TIME_IN_EVENT_BUFFER 10//seconds

    #define EVENT_RING_BUFFER_SIZE (size_t)(TIME_IN_EVENT_BUFFER * SAMPLING_RATE * NUM_CHANNELS)

    #define MAX_EVENT_SIZE (size_t)(NUM_CHANNELS * SAMPLING_RATE * 1.0/*seconds*/)

/// RINGBUFFER

    // these types are to ensure that certain ring buffer function 
    // are only accessible to one and not the other
    enum RING_BUFFER_TYPE{
        INTERNAL_RING_BUFFER,
        EVENT_RING_BUFFER,
        NUM_RING_BUFFER_TYPES
    };

/// INTAKE

    #define TIME_IN_INTERNAL_RING_BUFFER 4.0f//seconds
    #define INTERNAL_RING_BUFFER_SIZE (int)(TIME_IN_INTERNAL_RING_BUFFER * SAMPLING_RATE * NUM_CHANNELS)

    #define TIME_IN_WINDOW 1.2//seconds
    #define WINDOW_SIZE (size_t)(NUM_CHANNELS * SAMPLING_RATE * TIME_IN_WINDOW)

/// PROCESSING

    #define JSON_FILE_NAME "./ALGOS/RANDOM_FOREST/GOOD_FOREST_MODELS/maybe_this_one.json"

/// ALGOS/THRESHOLD

    #define REFRACTORY_PERIOD 0.3f //seconds
    #define REFRACTORY_SAMPLES (size_t)(SAMPLING_RATE * REFRACTORY_PERIOD)

    // best is 9.0f
    #define THRESH_MULT 7.0f

/// ALGOS/RANDOM_FOREST

    #define MIN_SAMPLES 5
    #define MAX_DEPTH 6

    enum NODE_TYPE{
        ROOT,
        BRANCH,
        LEAF
    };

    #define MAX_ROWS 1000
    #define MAX_COLS 10
    #define MAX_FIELD_LEN 64

    enum EVENT_TYPE{
        NOT_BLINK,
        BLINK,
        // SLOW_BLINK,
        // DOUBLE_BLINK,
        // HORIZONTAL_SACCADE
        // VERTICAL_SACCADE
        // MICRO_SLEEP
        // EYES_CLOSED
        // CLENCHED_JAW
        // RAISE_EYEBROWS
        // FURROW_EYEBROWS
        NUM_EVENT_TYPE
    };

/// LOG

    typedef enum{
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    }LOG_TYPE;

    typedef enum{
        THREAD_MASTER,
        THREAD_DATA_INTAKE,
        THREAD_DATA_PROCESSING,
        NONE,
    }THREAD_ID;

    #define LOG_FILE_NAME "vigilence.log"

/// LED 

    #define CHIPNAME "gpiochip0"
    #define LED_PIN 18  // GPIO pin number
    #define LED_DELAY_US 150000  


#endif
