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

bool getUartData(float data_point[NUM_CHANNELS]){}

void sendUartSignal(const enum TX_SIGNAL_TYPE signal_type){}


/*
#define _UART_BUFFER_SIZE_ 255
#define _UART_PROTO_HEADER_ '<'
#define _UART_PROTO_FOOTER_ '>'

#define _TRUE_ 0
#define _FALSE_ -1


typedef struct
{
    uint32_t count;
    char data[_UART_BUFFER_SIZE_];
}st_uart_data;


typedef struct
{
    uint32_t serial_fd;
    pthread_t th_recv;

    int32_t recv_temp_cnt;
    int32_t recv_buff_cnt;
    char recv_temp[_UART_BUFFER_SIZE_];
    char recv_buff[_UART_BUFFER_SIZE_];
    char send_buff[_UART_BUFFER_SIZE_ + 2];
    int32_t (*callback_function)(void *);

    st_uart_data data;

}st_uart;



int32_t serial_send(st_uart *s_uart,char *data);
int32_t uart_begin(st_uart *s_uart,char *device,int32_t baudrate,int32_t (*callback)(void *));
int32_t convert_baudrate(int32_t baudrate);
int32_t th_serial_recv(void *arg);

#endif // APP_STACK_UART_H*/

/*
int32_t uart_begin(st_uart *s_uart,char *device,int32_t baudrate,int32_t (*callback)(void *))
{
    struct termios termOptions;
    struct termios options;
    int32_t br = 0;

    if(callback != NULL)
    {
        s_uart->callback_function = callback;
    }

    if(baudrate >= 1200 && baudrate <= 230400)
    {
        br = convert_baudrate(baudrate);
    }
    else
    {
        printf ("Error : baudrate must be 1200 - 230400\r\n");
        return _FALSE_;
    }

    s_uart->serial_fd = -1;
    s_uart->serial_fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (s_uart->serial_fd == -1)
    {
        printf ("Error : open serial device: %s\r\n",device);
        perror("OPEN");
        return _FALSE_;
    }

    tcgetattr(s_uart->serial_fd, &options);
    options.c_cflag = br | CS8 | CLOCAL | CREAD;		//<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(s_uart->serial_fd, TCIFLUSH);

    if( tcsetattr(s_uart->serial_fd, TCSANOW, &options) < 0)
    {
        printf("ERROR :  Setup serial failed\r\n");
        return _FALSE_;
    }

    if (pthread_create(&s_uart->th_recv, NULL, (void *)th_serial_recv, (void *)(s_uart)) != _TRUE_)
    {
        printf("ERROR : initial thread receive serial failed\r\n");
        return _FALSE_;
    }

    return _TRUE_;

}

int32_t serial_send(st_uart *s_uart,char *data)
{
    if(strlen(data) < _UART_BUFFER_SIZE_)
    {
        memset(s_uart->send_buff,0,sizeof(s_uart->send_buff));
        sprintf(s_uart->send_buff,"<%s>",data);
        return write(s_uart->serial_fd, s_uart->send_buff, strlen(s_uart->send_buff));
    }

    return -1;
}

int32_t th_serial_recv(void *arg)
{
    st_uart *s_uart = (st_uart *)arg;
    fd_set rset;
    struct timeval tv;
    int32_t count = 0;
    uint8_t state = 0;
    uint8_t i = 0;


    while(1)
    {
        FD_ZERO(&rset);
        FD_SET(s_uart->serial_fd, &rset);
        tv.tv_sec = 30;
        tv.tv_usec = 0;

        count = select(s_uart->serial_fd + 1, &rset, NULL, NULL, &tv);

        if(count > 0)
        {
            memset(s_uart->recv_temp,0,_UART_BUFFER_SIZE_);
            s_uart->recv_temp_cnt = read(s_uart->serial_fd, (void*)s_uart->recv_temp, _UART_BUFFER_SIZE_);


            for(i = 0; i < s_uart->recv_temp_cnt; i++)
            {
                switch(state)
                {
                    case 0: //check header <
                        if(s_uart->recv_temp[i] == _UART_PROTO_HEADER_)
                        {
                            state = 1;
                            s_uart->recv_buff_cnt = 0;
                        }
                        break;
                    case 1: //copy data to buffer
                        if(s_uart->recv_temp[i] == _UART_PROTO_HEADER_)
                        {
                            s_uart->recv_buff_cnt = 0;
                        }
                        else if(s_uart->recv_temp[i] == _UART_PROTO_FOOTER_)
                        {
                            memset(s_uart->data.data,0,_UART_BUFFER_SIZE_);
                            memcpy(s_uart->data.data,s_uart->recv_buff,s_uart->recv_buff_cnt);
                            s_uart->data.count = s_uart->recv_buff_cnt;


                            state = 0;
                            memset(s_uart->recv_buff,0,_UART_BUFFER_SIZE_);

                            if(s_uart->callback_function != NULL)
                            {
                                (*s_uart->callback_function)(&s_uart->data);
                            }
                        }
                        else
                        {
                            s_uart->recv_buff[s_uart->recv_buff_cnt++] = s_uart->recv_temp[i];
                        }
                        break;

                }
                //printf("Out  State = %d, char = %c\r\n",state,s_uart->recv_temp[i]);
            }
        }
        else
        {
            if(s_uart->serial_fd < 0)
            {
                //close serial
                close(s_uart->serial_fd);
            }
        }
    }
}

int32_t convert_baudrate(int32_t baudrate)
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
}*/