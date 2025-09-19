#include "led.h"

int ledFlash(void){

    const int time_interval = LED_DELAY_US;

    struct gpiod_chip *chip;
    struct gpiod_line *line;
    

    // Open GPIO chip
    chip = gpiod_chip_open_by_name(CHIPNAME);
    if (!chip) {
        perror("Open chip failed");
        return 1;
    }

    // Get the line (GPIO)
    line = gpiod_chip_get_line(chip, LED_PIN);
    if (!line) {
        perror("Get line failed");
        gpiod_chip_close(chip);
        return 1;
    }

    // Request line as output, initial value 0 (LED off)
    int ret = gpiod_line_request_output(line, "led_blink", 0);
    if (ret < 0) {
        perror("Request line as output failed");
        gpiod_chip_close(chip);
        return 1;
    }

    // Blink the LED
    gpiod_line_set_value(line, 1);  // LED ON
    usleep(time_interval);
    gpiod_line_set_value(line, 0);  // LED OFF
    

    // Release the line and close chip
    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;

}


