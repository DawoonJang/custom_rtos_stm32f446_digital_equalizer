#ifndef __LED_H__
#define __LED_H__

#include "stm32f446xx.h"

#define NUM_LED0 12
#define NUM_LED1 13
#define NUM_LED2 14
#define NUM_LED3 15
#define NUM_LED4 0
#define NUM_LED5 1
#define NUM_LED6 2
#define NUM_LED7 3

void led_off(uint8_t led);

void led_on(uint8_t led);

#endif
