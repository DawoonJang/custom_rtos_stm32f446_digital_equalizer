#ifndef __OS_H__
#define __OS_H__

#include "stm32f446xx.h"
#include "task.h"

#define nullptr ((void *)0)
#define MAX_TASK 10
#define PRIO_HIGHEST (0)
#define PRIO_LOWEST (10)
#define NUM_PRIO (PRIO_LOWEST - PRIO_HIGHEST + 1)

#define STACK_SIZE (8 * 1024)
#define INIT_PSR (0x01000000)
#define TICK_MS (1)

#define IRQ_PRIORITY 0xE
#define NUM_IRQS 43 // 0 ~ 42

#define Macro_Set_Bit(dest, pos) ((dest) |= ((unsigned)0x1 << (pos)))

void switching_task(void);
uint8_t create_task(void (*ptask_func)(void *), void *const para, const int16_t prio, const uint16_t stack_size);
void init_os(void);
void start_os(void);
void update_delayed_tasks(void);
void delay_task(const uint32_t timeout);
uint8_t wait_signal(uint32_t *pdata, const uint32_t timeout);

#endif