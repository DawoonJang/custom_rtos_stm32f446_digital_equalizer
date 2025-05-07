//
// Created by Dawoon Jang on 25. 4. 25.
//

#include "lcd.h"

#include "stm32f4xx_it.h"

void clcd_pin_init(void)
{
	 HAL_GPIO_WritePin(GPIOC, LCD_D4_Pin | LCD_D5_Pin | LCD_D6_Pin | LCD_D7_Pin | LCD_E_Pin, GPIO_PIN_RESET);
	 HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET); // RS도 초기화

	 GPIO_InitTypeDef GPIO_InitStruct = {0};

	 GPIO_InitStruct.Pin	  = LCD_D4_Pin | LCD_D5_Pin | LCD_D6_Pin | LCD_D7_Pin | LCD_E_Pin;
	 GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	 GPIO_InitStruct.Pull  = GPIO_NOPULL;
	 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	 /*Configure GPIO pin : LCD_RS_Pin */
	 GPIO_InitStruct.Pin	  = LCD_RS_Pin;
	 GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	 GPIO_InitStruct.Pull  = GPIO_NOPULL;
	 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	 HAL_GPIO_Init(LCD_RS_GPIO_Port, &GPIO_InitStruct);
}
void ili9341_write_command(uint8_t cmd)
{
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);  // RS = 0 (Command)
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); // CS = 0
	 // HAL_SPI_FlushRxFifo(&hspi2, &cmd, 1, HAL_MAX_DELAY);
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // CS = 1
}

void ili9341_write_data(uint8_t data)
{
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);	  // RS = 1 (Data)
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); // CS = 0
	 // HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // CS = 1
}
