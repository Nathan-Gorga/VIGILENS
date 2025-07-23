
#define byte char

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


void createPacket(openbci_packet * packet, const byte channel1[3], const byte channel2[3]){

  const size_t packet_size = sizeof(openbci_packet);

  size_t count = 0;

  packet->fields.start_byte = START_BYTE;
  packet->fields.sample_number = 0xfc;


  packet->fields.channel_data[0][0] = 0x7f;
  packet->fields.channel_data[0][1] = 0xff;
  packet->fields.channel_data[0][2] = 0xff;



  packet->fields.channel_data[1][0] = 0x00;
  packet->fields.channel_data[1][1] = 0x00;
  packet->fields.channel_data[1][2] = 0xff;


  for(int j = 2; j < 8; j++){
    for(int i = 0; i < 3; i++){
      packet->fields.channel_data[j][i] = 0xff;
    }
  }
  packet->fields.stop_byte = STOP_BYTE_0;

  packet->fields.aux_data[0] = 0x01;
  packet->fields.aux_data[1] = 0x02;
  packet->fields.aux_data[2] = 0x03;
  packet->fields.aux_data[3] = 0x04;
  packet->fields.aux_data[4] = 0x05;
  packet->fields.aux_data[5] = 0x06;

}



const byte channel_data1[3] = {0xff, 0xff, 0xff};
const byte channel_data2[3] = {0x00, 0x00, 0x00};

void setup() {
  Serial.begin(9600); // Start UART at 9600 baud
  Serial.println("Hello, World");
}

void loop() {
  
  // Serial.print("Counter: ");//

  openbci_packet packet;

  createPacket(&packet, channel_data1, channel_data2);

  for(int i = 0; i < 33; i++){
    Serial.print(packet.packet[i]);
  }
  delay(1000); // Wait 1 second
}
