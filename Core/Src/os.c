#include "os.h"
#include "stm32f4xx_it.h"

ST_Task task_pool[MAX_TASK];

ST_Task *current_task_ptr = nullptr;

ST_TaskPool ready_task_pool[NUM_PRIO];
ST_TaskPool free_task_pool;
ST_TaskPool delay_task_pool;

static char os_stack[STACK_SIZE] __attribute__((__aligned__(8)));
static char *os_stack_ptr = os_stack + STACK_SIZE;
static char *os_stack_limit = os_stack;

static void _insert_ready_task(const uint8_t task_id);
static void _delete_ready_task(const uint8_t task_id);
static void _insert_free_task(const uint8_t task_id);
static void _insert_delay_task(const uint8_t task_id);
static void _delete_delay_task(const uint8_t task_id);

static void _insert_ready_task(const uint8_t task_id)
{
    ST_Task *ptask = &task_pool[task_id];
    ST_Task **head = &ready_task_pool[ptask->prio].head;

    if (*head == NULL)
    {
        *head = ptask;
        ptask->next = ptask;
        ptask->prev = ptask;
    }
    else
    {
        ptask->prev = (*head)->prev;
        ptask->next = *head;

        (*head)->prev->next = ptask;
        (*head)->prev = ptask;
    }
}

static void _delete_ready_task(const uint8_t task_id)
{
    ST_Task *ptask = &task_pool[task_id];
    ST_Task **head = &ready_task_pool[ptask->prio].head;

    if (*head == nullptr)
        return;

    ptask->prev->next = ptask->next;
    ptask->next->prev = ptask->prev;

    if (*head == ptask)
    {
        *head = (ptask->next == ptask) ? nullptr : ptask->next;
    }

    ptask->prev = ptask->next = nullptr;
}

static void _insert_free_task(const uint8_t task_id)
{
    ST_Task *ptask = &task_pool[task_id];
    ptask->next = free_task_pool.head;
    free_task_pool.head = ptask;
}

static void _insert_delay_task(const uint8_t task_id)
{
    ST_Task *ptask = &task_pool[task_id];

    if (delay_task_pool.head == nullptr)
    {
        delay_task_pool.head = ptask;
        ptask->prev = ptask->next = nullptr;
        return;
    }

    ptask->prev = nullptr;
    ptask->next = delay_task_pool.head;

    delay_task_pool.head = ptask;
    ptask->next->prev = ptask;
}

static void _delete_delay_task(const uint8_t task_id)
{
    ST_Task *ptask = &task_pool[task_id];

    if (ptask->prev != nullptr)
    {
        ptask->prev->next = ptask->next;
    }
    else
    {
        delay_task_pool.head = ptask->next;
    }

    if (ptask->next != nullptr)
    {
        ptask->next->prev = ptask->prev;
    }
}

void init_irq(void)
{
    uint32_t i;

    SCB->SHP[(uint8_t)SVCall_IRQn - 4] = 0xF << 4;
    SCB->SHP[(uint8_t)PendSV_IRQn - 4] = 0xF << 4;

    // Set IRQ priority
    for (i = 0; i < NUM_IRQS; ++i)
    {
        NVIC_SetPriority((IRQn_Type)i, IRQ_PRIORITY);
    }
}

void init_os(void)
{
    int i;

    free_task_pool.head = nullptr;
    delay_task_pool.head = nullptr;

    for (i = 0; i < MAX_TASK; i++)
    {
        task_pool[i].task_id = i;
        _insert_free_task(task_pool[i].task_id);
    }

    create_task(idle_task_func, nullptr, PRIO_LOWEST, 128);
}

void start_os(void)
{
    switching_task();
    init_irq();

    __asm__ volatile("svc #0");
}

static char *_allocate_stack(uint16_t stack_size)
{
    stack_size = (stack_size + 7) & ~0x7;

    char *new_stack = os_stack_ptr - stack_size;

    if (new_stack < os_stack_limit)
    {
        return nullptr;
    }

    os_stack_ptr = new_stack;
    return os_stack_ptr;
}

void switching_task(void)
{
    for (int prio = PRIO_HIGHEST; prio <= PRIO_LOWEST; prio++)
    {
        if (ready_task_pool[prio].head != nullptr)
        {
            current_task_ptr = ready_task_pool[prio].head;
            return;
        }
    }
}

ST_Task *get_free_task(void)
{
    ST_Task *task;

    if (free_task_pool.head == nullptr)
    {
        return nullptr;
    }

    task = free_task_pool.head;
    free_task_pool.head = task->next;

    if (free_task_pool.head != nullptr)
    {
        free_task_pool.head->prev = nullptr;
    }

    return task;
}

uint8_t create_task(void (*ptask_func)(void *), void *const para, const int16_t prio, const uint16_t stack_size)
{
    disable_interrupts();

    ST_Task *free_task_ptr = get_free_task();

    if (free_task_ptr == nullptr || stack_size <= 0 || prio < 0)
    {
        return -1;
    }

    free_task_ptr->top_of_stack = (unsigned long *)_allocate_stack(stack_size);

    if (free_task_ptr->top_of_stack == nullptr)
    {
        _insert_free_task(free_task_ptr->task_id);
        return -1;
    }

    free_task_ptr->prio = free_task_ptr->origin_prio = prio;
    free_task_ptr->state = TASK_READY;

    free_task_ptr->top_of_stack -= 16;
    free_task_ptr->top_of_stack[1] = 0x01010101; // r1 (디버깅용 dummy)
    free_task_ptr->top_of_stack[8] = (unsigned long)para;
    free_task_ptr->top_of_stack[14] = (unsigned long)ptask_func;
    free_task_ptr->top_of_stack[15] = INIT_PSR;
    _insert_ready_task(free_task_ptr->task_id);

    enable_interrupts();
    return free_task_ptr->task_id;
}

void update_delayed_tasks(void)
{
    disable_interrupts();

    const uint32_t cur_tick = HAL_GetTick();

    ST_Task *cur_task = delay_task_pool.head;
    ST_Task *next_task = cur_task->next;

    while (cur_task != nullptr)
    {
        next_task = cur_task->next;

        if (cur_task->task_timeout <= cur_tick)
        {
            cur_task->state = TASK_READY;
            cur_task->blocked_reason = BLOCKED_NONE;

            _delete_delay_task(cur_task->task_id);
            _insert_ready_task(cur_task->task_id);
        }

        cur_task = next_task;
    }

    enable_interrupts();
}

void delay_task(const uint32_t timeout)
{
    disable_interrupts();

    current_task_ptr->state = TASK_BLOCKED;
    current_task_ptr->blocked_reason = BLOCKED_SLEEP;
    current_task_ptr->task_timeout = HAL_GetTick() + timeout;

    _delete_ready_task(current_task_ptr->task_id);
    _insert_delay_task(current_task_ptr->task_id);

    trigger_context_switch();

    enable_interrupts();
}

void blocked_cur_task(const E_TaskBlockedReason blocked_reason, const uint32_t timeout)
{
    current_task_ptr->state = TASK_BLOCKED;
    current_task_ptr->blocked_reason = blocked_reason;
    current_task_ptr->task_timeout = HAL_GetTick() + timeout;

    _delete_ready_task(current_task_ptr->task_id);
    _insert_delay_task(current_task_ptr->task_id);
}

uint8_t wait_signal(uint32_t *pdata, const uint32_t timeout)
{
    E_TaskBlockedReason initial_blocked_reason;

    disable_interrupts();
    blocked_cur_task(BLOCKED_WAIT_SIGNAL, timeout);
    trigger_context_switch();
    enable_interrupts();

    initial_blocked_reason = current_task_ptr->blocked_reason;
    current_task_ptr->blocked_reason = BLOCKED_NONE;
    *pdata = current_task_ptr->received_signal;

    return (initial_blocked_reason == current_task_ptr->blocked_reason);
}

void send_signal(const uint8_t dest_task_id, const uint32_t signal)
{
    disable_interrupts();

    ST_Task *pdest_task = &task_pool[dest_task_id];

    if (pdest_task->state == TASK_BLOCKED && pdest_task->blocked_reason == BLOCKED_WAIT_SIGNAL)
    {
        pdest_task->received_signal = signal;
        pdest_task->state = TASK_READY;
        pdest_task->blocked_reason = BLOCKED_NONE;

        _delete_delay_task(dest_task_id);
        _insert_ready_task(dest_task_id);

        trigger_context_switch();
    }

    enable_interrupts();
}