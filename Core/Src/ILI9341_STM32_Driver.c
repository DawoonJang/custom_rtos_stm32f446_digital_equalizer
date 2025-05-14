//
// Created by https://github.com/eziya/STM32_HAL_ILI9341/blob/master/Src/ILI9341_STM32_Driver.c
//

#include "ILI9341_STM32_Driver.h"
#include "main.h"

volatile uint16_t LCD_HEIGHT = ILI9341_SCREEN_HEIGHT;
volatile uint16_t LCD_WIDTH  = ILI9341_SCREEN_WIDTH;

void HAL_SPI_TxCpltCallback(const SPI_HandleTypeDef *const hspi)
{
	 /* Deselect when Tx Complete */
	 if (hspi == HSPI_INSTANCE)
	 {
		  HAL_GPIO_WritePin(TFT_LED_GPIO_Port, TFT_LED_Pin, GPIO_PIN_SET);
	 }
}

static void ILI9341_SPI_Tx(uint8_t data)
{
	 while (!__HAL_SPI_GET_FLAG(HSPI_INSTANCE, SPI_FLAG_TXE))
	 {
		  ;
	 }

	 HAL_SPI_Transmit_DMA(HSPI_INSTANCE, &data, 1);
}

static void ILI9341_SPI_TxBuffer(const uint8_t *const buffer, const uint16_t len)
{
	 while (!__HAL_SPI_GET_FLAG(HSPI_INSTANCE, SPI_FLAG_TXE))
		  ;
	 HAL_SPI_Transmit_DMA(HSPI_INSTANCE, buffer, len);
}

void ILI9341_WriteCommand(const uint8_t cmd)
{
	 HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_RESET);	  // command
	 HAL_GPIO_WritePin(TFT_LED_GPIO_Port, TFT_LED_Pin, GPIO_PIN_RESET); // select
	 ILI9341_SPI_Tx(cmd);
}

void ILI9341_WriteData(const uint8_t data)
{
	 HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_SET);	  // data
	 HAL_GPIO_WritePin(TFT_LED_GPIO_Port, TFT_LED_Pin, GPIO_PIN_RESET); // select
	 ILI9341_SPI_Tx(data);
}

void ILI9341_WriteBuffer(const uint8_t *const buffer, const uint16_t len)
{
	 HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_SET);	  // data
	 HAL_GPIO_WritePin(TFT_LED_GPIO_Port, TFT_LED_Pin, GPIO_PIN_RESET); // select
	 ILI9341_SPI_TxBuffer(buffer, len);
}

void ILI9341_SetAddress(const uint16_t x1, const uint16_t y1, const uint16_t x2, const uint16_t y2)
{
	 uint8_t buffer[4];
	 buffer[0] = x1 >> 8;
	 buffer[1] = x1;
	 buffer[2] = x2 >> 8;
	 buffer[3] = x2;

	 ILI9341_WriteCommand(0x2A);
	 ILI9341_WriteBuffer(buffer, sizeof(buffer));

	 buffer[0] = y1 >> 8;
	 buffer[1] = y1;
	 buffer[2] = y2 >> 8;
	 buffer[3] = y2;

	 ILI9341_WriteCommand(0x2B);
	 ILI9341_WriteBuffer(buffer, sizeof(buffer));

	 ILI9341_WriteCommand(0x2C);
}

// void ILI9341_Reset(void)
// {
// 	 HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET); // Disable
// 	 HAL_Delay(10);
// 	 HAL_GPIO_WritePin(TFT_LED_GPIO_Port, TFT_LED_Pin, GPIO_PIN_RESET); // Select
// 	 HAL_Delay(10);
// 	 HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET); // Enable
// 	 HAL_GPIO_WritePin(TFT_LED_GPIO_Port, TFT_LED_Pin, GPIO_PIN_SET); // Deselect
// }
void ILI9341_Reset(void)
{
	 HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET);
	 HAL_Delay(50);
	 HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET);
	 HAL_Delay(120);
}

void ILI9341_Init(void)
{
	 ILI9341_Reset();

	 // Software Reset
	 ILI9341_WriteCommand(0x01);
	 HAL_Delay(10);

	 // Power Control A
	 ILI9341_WriteCommand(0xCB);
	 ILI9341_WriteData(0x39);
	 ILI9341_WriteData(0x2C);
	 ILI9341_WriteData(0x00);
	 ILI9341_WriteData(0x34);
	 ILI9341_WriteData(0x02);

	 // Power Control B
	 ILI9341_WriteCommand(0xCF);
	 ILI9341_WriteData(0x00);
	 ILI9341_WriteData(0xC1);
	 ILI9341_WriteData(0x30);

	 // Driver Timing Control A
	 ILI9341_WriteCommand(0xE8);
	 ILI9341_WriteData(0x85);
	 ILI9341_WriteData(0x00);
	 ILI9341_WriteData(0x78);

	 // Driver Timing Control B
	 ILI9341_WriteCommand(0xEA);
	 ILI9341_WriteData(0x00);
	 ILI9341_WriteData(0x00);

	 // Power On Sequence Control
	 ILI9341_WriteCommand(0xED);
	 ILI9341_WriteData(0x64);
	 ILI9341_WriteData(0x03);
	 ILI9341_WriteData(0x12);
	 ILI9341_WriteData(0x81);

	 // Pump Ratio Control
	 ILI9341_WriteCommand(0xF7);
	 ILI9341_WriteData(0x20);

	 // Power Control
	 ILI9341_WriteCommand(0xC0); // VRH
	 ILI9341_WriteData(0x23);

	 ILI9341_WriteCommand(0xC1); // SAP, BT
	 ILI9341_WriteData(0x10);

	 // VCM Control
	 ILI9341_WriteCommand(0xC5);
	 ILI9341_WriteData(0x3E);
	 ILI9341_WriteData(0x28);

	 ILI9341_WriteCommand(0xC7); // VCM Control 2
	 ILI9341_WriteData(0x86);

	 // Memory Access Control
	 ILI9341_WriteCommand(0x36);
	 ILI9341_WriteData(0x48);

	 // Pixel Format Set
	 ILI9341_WriteCommand(0x3A);
	 ILI9341_WriteData(0x55); // 16bit/pixel

	 // Frame Rate Control
	 ILI9341_WriteCommand(0xB1);
	 ILI9341_WriteData(0x00);
	 ILI9341_WriteData(0x18);

	 // Display Function Control
	 ILI9341_WriteCommand(0xB6);
	 ILI9341_WriteData(0x08);
	 ILI9341_WriteData(0x82);
	 ILI9341_WriteData(0x27);

	 // 3Gamma Function Disable
	 ILI9341_WriteCommand(0xF2);
	 ILI9341_WriteData(0x00);

	 // Gamma Curve Selected
	 ILI9341_WriteCommand(0x26);
	 ILI9341_WriteData(0x01);

	 // Positive Gamma Correction
	 ILI9341_WriteCommand(0xE0);
	 uint8_t posGamma[15] = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
	 for (int i = 0; i < 15; i++)
		  ILI9341_WriteData(posGamma[i]);

	 // Negative Gamma Correction
	 ILI9341_WriteCommand(0xE1);
	 uint8_t negGamma[15] = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
	 for (int i = 0; i < 15; i++)
		  ILI9341_WriteData(negGamma[i]);

	 // Exit Sleep Mode
	 ILI9341_WriteCommand(0x11);
	 HAL_Delay(100);

	 // Display ON
	 ILI9341_WriteCommand(0x29);

	 // Set rotation
	 ILI9341_SetRotation(SCREEN_VERTICAL_1);
}

void ILI9341_SetRotation(const uint8_t rotation)
{
	 ILI9341_WriteCommand(0x36);
	 HAL_Delay(1);

	 switch (rotation)
	 {
		  case SCREEN_VERTICAL_1:
				ILI9341_WriteData(0x40 | 0x08);
				LCD_WIDTH  = 240;
				LCD_HEIGHT = 320;
				break;
		  case SCREEN_HORIZONTAL_1:
				ILI9341_WriteData(0x20 | 0x08);
				LCD_WIDTH  = 320;
				LCD_HEIGHT = 240;
				break;
		  case SCREEN_VERTICAL_2:
				ILI9341_WriteData(0x80 | 0x08);
				LCD_WIDTH  = 240;
				LCD_HEIGHT = 320;
				break;
		  case SCREEN_HORIZONTAL_2:
				ILI9341_WriteData(0x40 | 0x80 | 0x20 | 0x08);
				LCD_WIDTH  = 320;
				LCD_HEIGHT = 240;
				break;
		  default:
				break;
	 }
}

void ILI9341_DrawColor(const uint16_t color)
{
	 uint8_t buffer[2] = {color >> 8, color};
	 ILI9341_WriteBuffer(buffer, sizeof(buffer));
}

void ILI9341_DrawColorBurst(const uint16_t color, const uint32_t size)
{
	 uint32_t BufferSize = 0;

	 if ((size * 2) < BURST_MAX_SIZE)
	 {
		  BufferSize = size;
	 }
	 else
	 {
		  BufferSize = BURST_MAX_SIZE;
	 }

	 HAL_GPIO_WritePin(TFT_RS_GPIO_Port, TFT_RS_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(TFT_LED_GPIO_Port, TFT_LED_Pin, GPIO_PIN_RESET);

	 const uint8_t chifted = color >> 8;
	 uint8_t			BurstBuffer[BufferSize];

	 for (uint32_t j = 0; j < BufferSize; j += 2)
	 {
		  BurstBuffer[j]		= chifted;
		  BurstBuffer[j + 1] = color;
	 }

	 const uint32_t SendingSize		  = size * 2;
	 const uint32_t SendingInBlock	  = SendingSize / BufferSize;
	 const uint32_t RemainderFromBlock = SendingSize % BufferSize;

	 if (SendingInBlock != 0)
	 {
		  for (uint32_t j = 0; j < (SendingInBlock); j++)
		  {
				HAL_SPI_Transmit(HSPI_INSTANCE, BurstBuffer, BufferSize, 10);
		  }
	 }

	 HAL_SPI_Transmit(HSPI_INSTANCE, BurstBuffer, RemainderFromBlock, 10);
	 HAL_GPIO_WritePin(TFT_LED_GPIO_Port, TFT_LED_Pin, GPIO_PIN_SET);
}

void ILI9341_FillScreen(const uint16_t color)
{
	 ILI9341_SetAddress(0, 0, LCD_WIDTH, LCD_HEIGHT);
	 ILI9341_DrawColorBurst(color, LCD_WIDTH * LCD_HEIGHT);
}

void ILI9341_DrawPixel(const uint16_t x, const uint16_t y, const uint16_t color)
{
	 if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
		  return;

	 uint8_t bufferX[4] = {x >> 8, x, (x + 1) >> 8, (x + 1)};
	 uint8_t bufferY[4] = {y >> 8, y, (y + 1) >> 8, (y + 1)};
	 uint8_t bufferC[2] = {color >> 8, color};

	 ILI9341_WriteCommand(0x2A);							// ADDRESS
	 ILI9341_WriteBuffer(bufferX, sizeof(bufferX)); // XDATA

	 ILI9341_WriteCommand(0x2B);							// ADDRESS
	 ILI9341_WriteBuffer(bufferY, sizeof(bufferY)); // YDATA

	 ILI9341_WriteCommand(0x2C);							// ADDRESS
	 ILI9341_WriteBuffer(bufferC, sizeof(bufferC)); // COLOR
}

void ILI9341_DrawRectangle(const uint16_t x, const uint16_t y, uint16_t width, uint16_t height, const uint16_t color)
{
	 if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
		  return;

	 if ((x + width - 1) >= LCD_WIDTH)
	 {
		  width = LCD_WIDTH - x;
	 }

	 if ((y + height - 1) >= LCD_HEIGHT)
	 {
		  height = LCD_HEIGHT - y;
	 }

	 ILI9341_SetAddress(x, y, x + width - 1, y + height - 1);
	 ILI9341_DrawColorBurst(color, height * width);
}

void ILI9341_DrawHLine(const uint16_t x, const uint16_t y, uint16_t width, const uint16_t color)
{
	 if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
		  return;

	 if ((x + width - 1) >= LCD_WIDTH)
	 {
		  width = LCD_WIDTH - x;
	 }

	 ILI9341_SetAddress(x, y, x + width - 1, y);
	 ILI9341_DrawColorBurst(color, width);
}

void ILI9341_DrawVLine(const uint16_t x, const uint16_t y, uint16_t height, const uint16_t color)
{
	 if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))
		  return;

	 if ((y + height - 1) >= LCD_HEIGHT)
	 {
		  height = LCD_HEIGHT - y;
	 }

	 ILI9341_SetAddress(x, y, x, y + height - 1);
	 ILI9341_DrawColorBurst(color, height);
}
