#include "uart.h"

int main() {

    if (!beginUART()) {

        printf("UART init failed\n");

        return 1;
    }

    printf("succesful initialization\n");

    float buffer[PACKET_BUFFER_SIZE * NUM_CHANNELS];

    while (1) {

        size_t n = getUARTData(buffer);

        if (n > 0) {

            for (size_t i = 0; i < n; i++) {

                printf("%f ", buffer[i]);
            }

            printf("\n");

        }
    }

    endUART();

    return 0;
}
