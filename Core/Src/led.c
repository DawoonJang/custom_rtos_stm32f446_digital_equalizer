#include "led.h"
#include "stm32f4xx_it.h"

void led_pin_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    HAL_GPIO_WritePin(GPIOB, PIN_LED1 | PIN_LED2 | PIN_LED3 | PIN_LED4, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = PIN_LED1 | PIN_LED2 | PIN_LED3 | PIN_LED4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOC, PIN_LED5 | PIN_LED6 | PIN_LED7, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = PIN_LED5 | PIN_LED6 | PIN_LED7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void led_on(unsigned int led)
{
    disable_interrupts();

    if (led == NUM_LED5 || led == NUM_LED6 || led == NUM_LED7 || led == NUM_LED8)
    {
        GPIOC->BSRR = (1 << led);
    }
    else
    {
        GPIOB->BSRR = (1 << led);
    }

    enable_interrupts();
}

void led_off(unsigned int led)
{
    disable_interrupts();

    if (led == NUM_LED5 || led == NUM_LED6 || led == NUM_LED7 || led == NUM_LED8)
    {
        GPIOC->BSRR = (1 << led) << 16;
    }
    else
    {
        GPIOB->BSRR = (1 << led) << 16;
    }

    enable_interrupts();
}
