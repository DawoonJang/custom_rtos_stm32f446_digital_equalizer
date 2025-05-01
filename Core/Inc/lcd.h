//
// Created by Dawoon Jang on 25. 4. 25.
//

#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

#define LCD_D4_Pin GPIO_PIN_0
#define LCD_D4_GPIO_Port GPIOC
#define LCD_D5_Pin GPIO_PIN_1
#define LCD_D5_GPIO_Port GPIOC
#define LCD_D6_Pin GPIO_PIN_2
#define LCD_D6_GPIO_Port GPIOC
#define LCD_D7_Pin GPIO_PIN_3
#define LCD_D7_GPIO_Port GPIOC

#define UART1_TXEN_Pin GPIO_PIN_5
#define UART1_TXEN_GPIO_Port GPIOC

#define UART4_TXEN_Pin GPIO_PIN_8

#define LCD_E_Pin GPIO_PIN_12
#define LCD_E_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_2
#define LCD_RS_GPIO_Port GPIOD


void clcd_pin_init(void);

void tft_glcd_pin_init(void);


#endif //LCD_H
