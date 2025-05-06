#include "led.h"
#include "stm32f4xx_it.h"

void led_on(const uint8_t led)
{
    disable_interrupts();

    if (led == NUM_LED4 || led == NUM_LED5 || led == NUM_LED6 || led == NUM_LED7)
    {
        GPIOC->BSRR = (1 << led);
    }
    else
    {
        GPIOB->BSRR = (1 << led);
    }

    enable_interrupts();
}

void led_off(const uint8_t led)
{
    disable_interrupts();

    if (led == NUM_LED4 || led == NUM_LED5 || led == NUM_LED6 || led == NUM_LED7)
    {
        GPIOC->BSRR = (1 << led) << 16;
    }
    else
    {
        GPIOB->BSRR = (1 << led) << 16;
    }

    enable_interrupts();
}
