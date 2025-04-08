#include "switch.h"

void switch_pin_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // SW1 (PD2)
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // 풀업으로 설정하면 눌렀을 때 LOW, 안 누르면 HIGH
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // SW2 (PA4)
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}