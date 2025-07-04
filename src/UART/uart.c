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


static inline float channelDataToFloat(const byte data[3]){
    return convertToFloat(interpret24BitToInt(data));
}
