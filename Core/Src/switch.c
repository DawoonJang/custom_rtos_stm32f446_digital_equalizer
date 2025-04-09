#include "switch.h"
#include "os.h"

extern uint8_t task3_id;

// Interrupt handler for SW1 (PD2)
void EXTI2_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);

        // Check if the button is pressed (LOW due to pull-up)
        if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET)
        {
            // Send a signal to the LED3 task
            send_signal(task3_id, 1); // Or any signal value you want to send
        }
    }
}

// Interrupt handler for SW2 (PA4)
void EXTI4_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);

        // Check if the button is pressed (LOW due to pull-up)
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)
        {
            // Send a signal to the LED3 task
            send_signal(task3_id, 2); // Or any signal value you want to send
        }
    }
}

// Configure external interrupts for the switches
void switch_pin_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable clock for GPIOD and GPIOA
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    // SW1 (PD2)
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; // Trigger on falling edge (button press)
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // SW2 (PA4)
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Enable and set EXTI interrupts to the lowest priority
    HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}