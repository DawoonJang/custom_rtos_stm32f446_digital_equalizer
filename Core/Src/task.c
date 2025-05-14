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
	 while (1)
	 {
		  TFT_DrawBox(50, 50, 100, 100, 0xF800);
	 }
}
