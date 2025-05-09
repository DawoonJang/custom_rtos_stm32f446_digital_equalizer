#include "task.h"

#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"
#include "os.h"

void idle_task_func(void *param)
{
	 while (1)
	 {
		  ILI9341_FillScreen(WHITE);
	 }
}

void led1_task_func(void *param)
{
	 static uint8_t flag = 1;

	 while (1)
	 {
		  if (flag)
		  {
				led_on(NUM_LED6);
		  }
		  else
		  {
				led_off(NUM_LED6);
		  }

		  flag = !flag;
		  delay_task(1000);
	 }
}

void led2_task_func(void *param)
{
	 static uint8_t flag;

	 while (1)
	 {
		  if (flag)
		  {
				led_on(NUM_LED7);
		  }
		  else
		  {
				led_off(NUM_LED7);
		  }

		  flag = !flag;
		  delay_task(1000);
	 }
}

void led3_task_func(void *param)
{

	 static uint8_t sw1_flag; // Flag for SW1 (NUM_LED5)
	 static uint8_t sw2_flag; // Flag for SW2 (NUM_LED6)

	 while (1)
	 {
		  uint32_t signal = 0;

		  if (wait_signal(&signal, 2000))
		  {
				if (signal == 1) // SW1 pressed
				{
					 sw1_flag = !sw1_flag;
				}
				if (signal == 2) // SW2 pressed
				{
					 sw2_flag = !sw2_flag;
				}
		  }
		  else
		  {
				// timeout occurred, add code here if you want to change led states on
				// timeout.
		  }

		  if (sw1_flag)
		  {
				led_on(NUM_LED5);
		  }
		  else
		  {
				led_off(NUM_LED5);
		  }

		  if (sw2_flag)
		  {
				led_on(NUM_LED4);
		  }
		  else
		  {
				led_off(NUM_LED4);
		  }
	 }
}

void lcd_task_func(void *param)
{

	 static uint16_t x = 0;
	 static uint16_t y = 0;
	 static char	  BufferText[40];


	 while (1)
	 {

		  /* USER CODE END WHILE */
		  ILI9341_FillScreen(WHITE);
		  ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
		  ILI9341_DrawText("Counting multiple segments at once", FONT2, 10, 10, BLACK, WHITE);
		  HAL_Delay(2000);
		  ILI9341_FillScreen(WHITE);

		  for (uint16_t i = 0; i <= 10; i++)
		  {
				ILI9341_DrawText(BufferText, FONT3, 10, 10, BLACK, WHITE);
				ILI9341_DrawText(BufferText, FONT3, 10, 30, BLUE, WHITE);
				ILI9341_DrawText(BufferText, FONT3, 10, 50, RED, WHITE);
				ILI9341_DrawText(BufferText, FONT3, 10, 70, GREEN, WHITE);
				ILI9341_DrawText(BufferText, FONT3, 10, 90, BLACK, WHITE);
				ILI9341_DrawText(BufferText, FONT3, 10, 110, BLUE, WHITE);
				ILI9341_DrawText(BufferText, FONT3, 10, 130, RED, WHITE);
				ILI9341_DrawText(BufferText, FONT3, 10, 150, GREEN, WHITE);
				ILI9341_DrawText(BufferText, FONT3, 10, 170, WHITE, BLACK);
				ILI9341_DrawText(BufferText, FONT3, 10, 190, BLUE, BLACK);
				ILI9341_DrawText(BufferText, FONT3, 10, 210, RED, BLACK);
		  }
	 }
}
