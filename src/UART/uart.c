#include "uart.h"


static int32_t interpret24BitToInt(const byte data[3]){//TESTME : test this function thoroughly
    
    int32_t Int = (  
     ((0xFF & data[0]) << 16) |  //TESTME : there might be a problem here regarding MSB
     ((0xFF & data[1]) << 8) |   
     (0xFF & data[2])  
    );  

    if ((Int & 0x00800000) > 0) {  
      Int |= 0xFF000000;  
    } else {  
      Int &= 0x00FFFFFF;  
    }

    return Int;
}

static inline float convertToFloat(const int32_t value){
    return (float)(value * SCALE_FACTOR);
}


static inline float channelDataToFloat(const byte data[3]){//TESTME
    return convertToFloat(interpret24BitToInt(data));
}


static void getChannelDataFromPacket(const openbci_packet packet, float data_point[NUM_CHANNELS]){
    
    for(int i = 0; i < NUM_CHANNELS; i++){
        
        data_point[i] = channelDataToFloat(packet.fields.channel_data[i]);
        
    }
}

static int32_t convertBaudrate(const int32_t baudrate)
{
    switch(baudrate)
    {
        case 1200: return B1200;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
    }
    
    return B9600;
}
static bool openSerialFileDescriptor(void){

    UART_fd = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if(UART_fd == -1)
    {
        printf ("Error : open serial device: %s\r\n",SERIAL_DEVICE);
        perror("OPEN");
        return false;
    }
    
}

static bool setTermiosOptions(void){

    struct termios term_options;

    int32_t baudrate = convertBaudrate(UART_BAUDRATE);

    tcgetattr(UART_fd, &term_options);

    term_options.c_cflag = baudrate | CS8 | CLOCAL | CREAD;		//<Set baud rate
    term_options.c_iflag = IGNPAR;
    term_options.c_oflag = 0;
    term_options.c_lflag = 0;

    tcflush(UART_fd, TCIFLUSH);

    if( tcsetattr(UART_fd, TCSANOW, &term_options) < 0)
    {
        printf("ERROR :  Setup serial failed\r\n");
        return false;
    }
}


bool beginUART(void){
    
    if(!openSerialFileDescriptor()) return false;

    if(!setTermiosOptions()) return false;

    // TRY : without another thread, keep it simple, I think the OS can do the heavy lifting and I just handle polling

    // if (pthread_create(&s_uart->th_recv, NULL, (void *)th_serial_recv, (void *)(s_uart)) != _TRUE_)
    // {
    //     printf("ERROR : initial thread receive serial failed\r\n");
    //     return _FALSE_;
    // }

    return true;
}

void endUART(void){

    close(UART_fd);

}

static size_t getPacketsFromUARTBuffer(const byte buffer[], const size_t size_read, openbci_packet packets[]){//TESTME

    const size_t packet_size = sizeof(openbci_packet);

    size_t count = 0;

    for(int i = 0; i < size_read; i++){

        if(buffer[i] == START_BYTE){

            memcpy(&packets[count++], &buffer[i], packet_size);

            i += packet_size - 1;
        }

    }

    return count;
}

bool getUARTData(float data_point[NUM_CHANNELS]){//TESTME : test this function thoroughly
    //CLEANME
    
    openbci_packet packets[UART_BUFFER_SIZE / sizeof(openbci_packet)];
    
    fd_set read_UART_fd;
    FD_ZERO(&read_UART_fd);
    FD_SET(UART_fd, &read_UART_fd);

    byte receiver_buffer[UART_BUFFER_SIZE] = {0.0f};

    const short impatience = 2;

    struct timeval timeout = {0, (__suseconds_t)( SAMPLE_TIME_uS / impatience )};

    const int ready = select(UART_fd + 1, &read_UART_fd, NULL, NULL, &timeout);

    if(ready > 0 && FD_ISSET(UART_fd, &read_UART_fd)){

        const size_t size_read = read(UART_fd, receiver_buffer, UART_BUFFER_SIZE);

        if(size_read > 0){

            const size_t num_packets = getPacketsFromUARTBuffer(receiver_buffer,size_read,packets);

            for(int i = 0; i < num_packets; i++){

                getChannelDataFromPacket(packets[i], data_point);
                                
            }
        }

        return true;
    }

    return false;
}

bool sendUARTSignal(const enum TX_SIGNAL_TYPE signal_type){

    //TODO : is there a specific format to SEND to openBCI?
    
    const size_t size_written = sizeof(byte);

    if(write(UART_fd, (byte)signal_type, sizeof(byte)) != size_written) return false;
    
    return false;
}

