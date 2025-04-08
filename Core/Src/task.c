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
    uint8_t signal = 0;

    while (1)
    {
        if (wait_signal(&signal, 5000))

        {
            led_on(NUM_LED5);
            led_on(NUM_LED6);
        }
        else
        {
            led_off(NUM_LED5);
            led_off(NUM_LED6);
        }

        flag = !flag;
    }
}
