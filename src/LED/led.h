#ifndef LED_H
#define LED_H

#include "../globaldefinition.h"
#include <fcntl.h>
#define GPIO_PIN "18"


void gpio_export();

void gpio_unexport();

void gpio_direction(const char* dir);

void gpio_write(const char* value);

#endif