#ifndef __LED_H__
#define __LED_H__

#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"

#define PIN_LED1 GPIO_PIN_12
#define PIN_LED2 GPIO_PIN_13
#define PIN_LED3 GPIO_PIN_14
#define PIN_LED4 GPIO_PIN_15
#define PIN_LED5 GPIO_PIN_0
#define PIN_LED6 GPIO_PIN_1
#define PIN_LED7 GPIO_PIN_2
#define PIN_LED8 GPIO_PIN_3

#define NUM_LED1 12
#define NUM_LED2 13
#define NUM_LED3 14
#define NUM_LED4 15
#define NUM_LED5 0
#define NUM_LED6 1
#define NUM_LED7 2
#define NUM_LED8 2

void led_pin_init(void);

void led_off(uint8_t led);

void led_on(uint8_t led);

#endif
