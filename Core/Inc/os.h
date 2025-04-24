#ifndef __OS_H__
#define __OS_H__

#include "stm32f446xx.h"
#include "task.h"

#define nullptr ((void *)0)
#define MAX_TASK 10
#define PRIO_HIGHEST (0)
#define PRIO_LOWEST (10)
#define NUM_PRIO (PRIO_LOWEST - PRIO_HIGHEST + 1)

#define MAX_QUEUE 10
#define MAX_MUTEX 10

#define STACK_SIZE (8 * 1024)
#define QUEUE_STACK_SIZE (8 * 1024)

#define INIT_PROCESSOR_STATE_REGISTER (0x01000000)
#define DEBUG_DUMMY_R1 (0x01010101)
#define TICK_MS (1)

#define IRQ_PRIORITY 0xE
#define NUM_IRQS 43 // 0 ~ 42

#define Macro_Set_Bit(dest, pos) ((dest) |= ((unsigned)0x1 << (pos)))

typedef struct _queue
{
    uint32_t receiver_task_id;
    uint8_t *front;
    uint8_t *rear;
    uint8_t *buffer;
    uint8_t *buffer_end;

    uint32_t element_size;
    uint32_t capacity;
} ST_QUEUE;

typedef struct _mutex
{
    uint8_t is_lock;
    uint8_t owner_id;
    uint8_t waiting_task[MAX_TASK];
} ST_MUTEX;

void switching_task(void);

uint8_t create_task(void (*ptask_func)(void *), void *const para, const int16_t prio, const uint16_t stack_size);

void init_os(void);

void start_os(void);

void update_delayed_tasks(void);

void delay_task(const uint32_t timeout);

uint8_t wait_signal(uint32_t *pdata, const uint32_t timeout);

void send_signal(const uint8_t dest_task_id, const uint32_t signal);

#endif
