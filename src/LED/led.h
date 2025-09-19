#ifndef LED_H
#define LED_H

#include "../globaldefinition.h"
#include <fcntl.h>
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

int ledFlash(void);

#endif

