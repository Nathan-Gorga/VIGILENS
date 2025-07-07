#include "uart.h"



static int32_t interpret24BitToInt(const byte data[3]){//DONTTOUCH
    
    int32_t Int = (  
     ((0xFF & data[0]) << 16) |
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


static inline float convertToFloat(const int32_t value){//DONTTOUCH
    return (float)(value * SCALE_FACTOR);
}



static inline float channelDataToFloat(const byte data[3]){//DONTTOUCH
    return convertToFloat(interpret24BitToInt(data));
}


static void getChannelDataFromPacket(const openbci_packet packet, float data_point[NUM_CHANNELS]){//DONTTOUCH
    
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
    return true;
}

static bool setTermiosOptions(void){//TESTME
    
    struct termios term_options;

    int32_t baudrate = convertBaudrate(UART_BAUDRATE);

    if(tcgetattr(UART_fd, &term_options)) return false;

    if(cfsetispeed(&term_options, baudrate) || cfsetospeed(&term_options, baudrate)) return false;
    

    //FIXME  : sync openBCI UART settings with RPi
    term_options.c_cflag &= ~PARENB; // No parity
    term_options.c_cflag &= ~CSTOPB; // 1 stop bit
    term_options.c_cflag &= ~CSIZE;  // Clear size bits
    term_options.c_cflag |= CS8;     // 8 data bits
    term_options.c_cflag |= CLOCAL | CREAD;

    term_options.c_iflag = IGNPAR;
    term_options.c_oflag = 0;
    term_options.c_lflag = 0;

    if(tcflush(UART_fd, TCIFLUSH)) return false;

    if( tcsetattr(UART_fd, TCSANOW, &term_options) < 0) return false;

    return true;
}


bool beginUART(void){//TESTME
    
    if(!openSerialFileDescriptor()) return false;

    if(!setTermiosOptions()) return false;

    return true;
}


void endUART(void){

    close(UART_fd);

}


static size_t getPacketsFromUARTBuffer(const byte buffer[], const size_t size_read, openbci_packet packets[]){//DONTTOUCH

    const size_t packet_size = sizeof(openbci_packet);

    size_t count = 0;

    for(int i = 0; i < size_read; i++){
        
        if(buffer[i] == START_BYTE){
            
            memcpy(&packets[count++], &buffer[i], packet_size);
            
            printf("count : %d\n", count);
            
            i += packet_size - 1;
        }
    }

    return count;
}

size_t getUARTData(float data_points[PACKET_BUFFER_SIZE]){
    
    //TESTME : test this function thoroughly
    
    openbci_packet packets[PACKET_BUFFER_SIZE];
    
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
                                                            //  \/ this is to send the array pointer shifted, as to not overwrite previous data
                getChannelDataFromPacket(packets[i], data_points + (NUM_CHANNELS * i));
                                
            }
            return num_packets * NUM_CHANNELS;//num of elements in data_point
        }
    }

    return 0;
}

bool sendUARTSignal(const enum TX_SIGNAL_TYPE signal_type){//TESTME

    // TODO : openBCI sends $$$ when ready (before any signal), add this to the function
    
    const size_t size_written = sizeof(byte);

    if(write(UART_fd, (byte)signal_type, sizeof(byte)) != size_written) return false;
    
    return true;
}
