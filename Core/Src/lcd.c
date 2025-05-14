//
// Created by Dawoon Jang on 25. 4. 25.
//

#include "lcd.h"
#include "main.h"
#include "stm32f4xx_it.h"

extern SPI_HandleTypeDef hspi2;


void TFT_Reset(void)
{
	 HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET);
	 HAL_Delay(50);
	 HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET);
	 HAL_Delay(120);
}

void TFT_SendCommand(uint8_t cmd)
{
	 HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_RESET); // DC low = command
	 HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
}

void TFT_SendData(uint8_t data)
{
	 HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_SET); // DC high = data
	 HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
}

void TFT_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	 TFT_SendCommand(0x2A); // Column addr set
	 TFT_SendData(x0 >> 8);
	 TFT_SendData(x0 & 0xFF);
	 TFT_SendData(x1 >> 8);
	 TFT_SendData(x1 & 0xFF);

	 TFT_SendCommand(0x2B); // Row addr set
	 TFT_SendData(y0 >> 8);
	 TFT_SendData(y0 & 0xFF);
	 TFT_SendData(y1 >> 8);
	 TFT_SendData(y1 & 0xFF);

	 TFT_SendCommand(0x2C); // Write to RAM
}

void TFT_DrawBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	 TFT_SetAddressWindow(x, y, x + w - 1, y + h - 1);

	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); // data mode

	 for (uint32_t i = 0; i < w * h; i++)
	 {
		  uint8_t data[2] = {color >> 8, color & 0xFF};
		  HAL_SPI_Transmit(&hspi2, data, 2, HAL_MAX_DELAY);
	 }
}
