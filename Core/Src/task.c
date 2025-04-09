#include "task.h"
#include "os.h"
#include "stm32f4xx_it.h"

void idle_task_func(void *param)
{
    while (1)
    {
        ;
    }
}

void led1_task_func(void *param)
{
    static uint8_t flag = 1;

    while (1)
    {
        if (flag)
        {
            led_on(NUM_LED3);
            led_on(NUM_LED4);
        }
        else
        {
            led_off(NUM_LED3);
            led_off(NUM_LED4);
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
            led_on(NUM_LED1);
            led_on(NUM_LED2);
        }
        else
        {
            led_off(NUM_LED1);
            led_off(NUM_LED2);
        }

        flag = !flag;
        delay_task(1000);
    }
}
void led3_task_func(void *param)
{
    uint32_t signal = 0;

    static uint8_t sw1_flag; // Flag for SW1 (NUM_LED5)
    static uint8_t sw2_flag; // Flag for SW2 (NUM_LED6)

    while (1)
    {
        if (wait_signal(&signal, 5000))
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
            // timeout occurred, add code here if you want to change led states on timeout.
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
            led_on(NUM_LED6);
        }
        else
        {
            led_off(NUM_LED6);
        }
    }
}
