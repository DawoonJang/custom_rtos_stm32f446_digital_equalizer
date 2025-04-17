#ifndef __TASK_H__
#define __TASK_H__

#include "led.h"

#define TRUE 1
#define FALSE 0

typedef enum
{
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED
} E_TaskState;

typedef enum
{
    BLOCKED_NONE,
    BLOCKED_SLEEP,
    BLOCKED_WAIT_SIGNAL
} E_TaskBlockedReason;

typedef struct _task
{
    uint32_t *top_of_stack; // 8: pparam 14: pfunc 15:PSR
    uint32_t task_id;
    uint32_t prio;
    uint32_t origin_prio;
    uint32_t task_timeout;
    uint32_t received_signal;

    struct _task *next;
    struct _task *prev;
    E_TaskState state;
    E_TaskBlockedReason blocked_reason;
} ST_Task;

typedef struct _task_pool
{
    ST_Task *head;
} ST_TaskPool;

void idle_task_func(void *param);

void led1_task_func(void *param);
void led2_task_func(void *param);
void led3_task_func(void *param);

#endif