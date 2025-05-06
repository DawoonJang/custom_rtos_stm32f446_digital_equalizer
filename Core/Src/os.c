#include "os.h"

#include <stdbool.h>

#include "lcd.h"

#include <string.h>
#include "stm32f4xx_it.h"


ST_Task task_pool[MAX_TASK];

ST_Task *current_task_ptr = nullptr;

ST_TaskPool ready_task_pool[NUM_PRIO];
ST_TaskPool free_task_pool;
ST_TaskPool delay_task_pool;

static ST_QUEUE queue_pool[MAX_QUEUE];
static ST_MUTEX mutex_pool[MAX_MUTEX];

static uint8_t	 os_stack[STACK_SIZE] __attribute__((__aligned__(8)));
static uint8_t *os_stack_ptr	 = os_stack + STACK_SIZE;
static uint8_t *os_stack_limit = os_stack;

static uint8_t os_queue_stack[QUEUE_STACK_SIZE] __attribute__((__aligned__(8)));


static void s_insert_ready_task(uint8_t task_id);

static void s_delete_ready_task(uint8_t task_id);

static void s_insert_free_task(uint8_t task_id);

static void s_insert_delay_task(uint8_t task_id);

static void s_delete_delay_task(uint8_t task_id);

static void s_insert_ready_task(const uint8_t task_id)
{
	 ST_Task	 *ptask = &task_pool[task_id];
	 ST_Task **head  = &ready_task_pool[ptask->prio].head;

	 if (*head == NULL)
	 {
		  *head		  = ptask;
		  ptask->next = ptask;
		  ptask->prev = ptask;
	 }
	 else
	 {
		  ptask->prev = (*head)->prev;
		  ptask->next = *head;

		  (*head)->prev->next = ptask;
		  (*head)->prev		 = ptask;
	 }
}

static void s_delete_ready_task(const uint8_t task_id)
{
	 ST_Task	 *ptask = &task_pool[task_id];
	 ST_Task **head  = &ready_task_pool[ptask->prio].head;

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

static void s_insert_free_task(const uint8_t task_id)
{
	 ST_Task *ptask		= &task_pool[task_id];
	 ptask->next			= free_task_pool.head;
	 free_task_pool.head = ptask;
}

static void s_insert_delay_task(const uint8_t task_id)
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
	 ptask->next->prev	 = ptask;
}

static void s_delete_delay_task(const uint8_t task_id)
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
	 SCB->SHP[(uint8_t) SVCall_IRQn - 4] = 0xF << 4;
	 SCB->SHP[(uint8_t) PendSV_IRQn - 4] = 0xF << 4;

	 // Set IRQ priority
	 for (uint32_t i = 0; i < NUM_IRQS; ++i)
	 {
		  NVIC_SetPriority((IRQn_Type) i, IRQ_PRIORITY);
	 }
}

void init_os(void)
{
	 free_task_pool.head	 = nullptr;
	 delay_task_pool.head = nullptr;

	 for (int i = 0; i < MAX_TASK; i++)
	 {
		  task_pool[i].task_id = i;
		  s_insert_free_task(task_pool[i].task_id);
	 }

	 create_task(idle_task_func, nullptr, PRIO_LOWEST, 128);
}

void start_os(void)
{
	 switching_task();
	 init_irq();
	 // tft_glcd_pin_init();

	 __asm__ volatile("svc #0");
}

static uint8_t *s_allocate_stack(uint16_t stack_size)
{
	 stack_size = (stack_size + 7) & ~0x7;

	 uint8_t *new_stack = os_stack_ptr - stack_size;

	 if (new_stack < os_stack_limit)
	 {
		  return nullptr;
	 }

	 os_stack_ptr = new_stack;
	 return os_stack_ptr;
}

static uint8_t *s_allocate_queue_momory(uint16_t queue_size)
{
	 static uint8_t *queue_stack_ptr = os_queue_stack;

	 queue_size = (queue_size + 7) & ~0x7;

	 if (queue_stack_ptr + queue_size >= os_queue_stack + QUEUE_STACK_SIZE)
	 {
		  return nullptr;
	 }

	 uint8_t *ret = queue_stack_ptr;
	 queue_stack_ptr += queue_size;

	 return ret;
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
	 if (free_task_pool.head == nullptr)
	 {
		  return nullptr;
	 }

	 ST_Task *task			= free_task_pool.head;
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

	 if (prio < PRIO_HIGHEST || prio > PRIO_LOWEST)
	 {
		  enable_interrupts();
		  return -1; // Invalid priority
	 }

	 if (stack_size <= 0)
	 {
		  enable_interrupts();
		  return -1; // Invalid stack size
	 }

	 ST_Task *free_task_ptr = get_free_task();

	 if (free_task_ptr == nullptr)
	 {
		  enable_interrupts();
		  return -1; // No free task available
	 }

	 free_task_ptr->top_of_stack = (unsigned long *) s_allocate_stack(stack_size);

	 if (free_task_ptr->top_of_stack == nullptr)
	 {
		  s_insert_free_task(free_task_ptr->task_id);
		  enable_interrupts();
		  return -1; // Stack allocation failed
	 }

	 free_task_ptr->prio = free_task_ptr->origin_prio = prio;
	 free_task_ptr->state									  = STATE_READY;
	 free_task_ptr->blocked_reason						  = BLOCKED_NONE;

	 free_task_ptr->top_of_stack -= 16;
	 free_task_ptr->top_of_stack[1]	= DEBUG_DUMMY_R1;
	 free_task_ptr->top_of_stack[8]	= (unsigned long) para;
	 free_task_ptr->top_of_stack[14] = (unsigned long) ptask_func;
	 free_task_ptr->top_of_stack[15] = INIT_PROCESSOR_STATE_REGISTER;
	 s_insert_ready_task(free_task_ptr->task_id);

	 enable_interrupts();
	 return free_task_ptr->task_id;
}

void update_delayed_tasks(void)
{
	 disable_interrupts();

	 const uint32_t cur_tick = HAL_GetTick();

	 ST_Task *cur_task  = delay_task_pool.head;
	 ST_Task *next_task = cur_task->next;

	 while (cur_task != nullptr)
	 {
		  next_task = cur_task->next;

		  if (cur_task->task_timeout <= cur_tick)
		  {
				cur_task->state			 = STATE_READY;
				cur_task->blocked_reason = BLOCKED_NONE;

				s_delete_delay_task(cur_task->task_id);
				s_insert_ready_task(cur_task->task_id);
		  }

		  cur_task = next_task;
	 }

	 enable_interrupts();
}

void delay_task(const uint32_t timeout)
{
	 disable_interrupts();

	 current_task_ptr->state			 = STATE_BLOCKED;
	 current_task_ptr->blocked_reason = BLOCKED_SLEEP;
	 current_task_ptr->task_timeout	 = HAL_GetTick() + timeout;

	 s_delete_ready_task(current_task_ptr->task_id);
	 s_insert_delay_task(current_task_ptr->task_id);

	 trigger_context_switch();

	 enable_interrupts();
}

void blocked_cur_task(const E_TaskBlockedReason blocked_reason, const uint32_t timeout)
{
	 current_task_ptr->state			 = STATE_BLOCKED;
	 current_task_ptr->blocked_reason = blocked_reason;
	 current_task_ptr->task_timeout	 = HAL_GetTick() + timeout;

	 s_delete_ready_task(current_task_ptr->task_id);
	 s_insert_delay_task(current_task_ptr->task_id);
}

uint8_t wait_signal(uint32_t *const pdata, const uint32_t timeout)
{
	 const E_TaskBlockedReason initial_blocked_reason = current_task_ptr->blocked_reason;

	 disable_interrupts();

	 blocked_cur_task(BLOCKED_WAIT_SIGNAL, timeout);

	 trigger_context_switch();
	 enable_interrupts();

	 if (current_task_ptr->blocked_reason == BLOCKED_NONE)
	 {
		  return false;
	 }

	 current_task_ptr->blocked_reason = BLOCKED_NONE;
	 *pdata									 = current_task_ptr->received_signal;

	 return (initial_blocked_reason == BLOCKED_WAIT_SIGNAL);
}

void send_signal(const uint8_t dest_task_id, const uint32_t signal)
{
	 disable_interrupts();

	 ST_Task *pdest_task = &task_pool[dest_task_id];

	 if (pdest_task->state == STATE_BLOCKED && pdest_task->blocked_reason == BLOCKED_WAIT_SIGNAL)
	 {
		  pdest_task->received_signal = signal;
		  pdest_task->state				= STATE_READY;
		  pdest_task->blocked_reason	= BLOCKED_NONE;

		  s_delete_delay_task(dest_task_id);
		  s_insert_ready_task(dest_task_id);

		  trigger_context_switch();
	 }

	 enable_interrupts();
}

uint32_t create_queue(const uint32_t capacity, const uint32_t element_size)
{
	 disable_interrupts();
	 static uint8_t q_idx;

	 if (capacity <= 0 || element_size <= 0)
	 {
		  return -1;
	 }

	 uint8_t q_id = q_idx++;

	 queue_pool[q_id].buffer = s_allocate_queue_momory((capacity + 1) * element_size);

	 if (queue_pool[q_id].buffer == nullptr)
	 {
		  return -1;
	 }

	 queue_pool[q_id].element_size = element_size;
	 queue_pool[q_id].capacity		 = capacity;
	 queue_pool[q_id].front = queue_pool[q_id].rear = queue_pool[q_id].buffer;
	 queue_pool[q_id].buffer_end							= queue_pool[q_id].buffer + ((capacity + 1) * element_size);
	 queue_pool[q_id].receiver_task_id					= current_task_ptr->task_id;

	 return q_id;

	 enable_interrupts();
}

static void s_move_queue_ptr(uint8_t **q_ptr, const uint32_t queue_id)
{
	 const ST_QUEUE q = queue_pool[queue_id];

	 *q_ptr += q.element_size;

	 if (*q_ptr >= q.buffer_end)
	 {
		  *q_ptr = q.buffer;
	 }
}

uint8_t enqueue(const uint32_t queue_id, const uint32_t *const pdata)
{
	 disable_interrupts();
	 ST_QUEUE *q_ptr = &queue_pool[queue_id];

	 if (q_ptr->front == nullptr
		  // ||is_queue_full()
	 )
	 {
		  enable_interrupts();
		  return FALSE;
	 }

	 ST_Task *received_task_ptr = &task_pool[q_ptr->receiver_task_id];
	 memcpy(q_ptr->rear, pdata, q_ptr->element_size);
	 s_move_queue_ptr(&q_ptr->rear, queue_id);

	 if (received_task_ptr->blocked_reason == BLOCKED_WAIT_SIGNAL)
	 {
		  received_task_ptr->state				= STATE_READY;
		  received_task_ptr->blocked_reason = BLOCKED_NONE;

		  s_delete_delay_task(received_task_ptr->task_id);
		  s_insert_ready_task(received_task_ptr->task_id);
		  trigger_context_switch();
	 }

	 enable_interrupts();

	 return TRUE;
}

uint8_t dequeue(const uint32_t queue_id, uint32_t *const pdata, const uint32_t timeout)
{
	 ST_QUEUE *q_ptr = &queue_pool[queue_id];

	 disable_interrupts();

	 if (queue_id >= MAX_QUEUE || q_ptr->buffer == nullptr || q_ptr->receiver_task_id != current_task_ptr->task_id)
	 {
		  enable_interrupts();

		  return FALSE;
	 }

	 if (q_ptr->front == q_ptr->rear)
	 {
		  enable_interrupts();

		  memcpy(pdata, q_ptr->front, q_ptr->element_size);
		  s_move_queue_ptr(&q_ptr->front, queue_id);
		  return TRUE;
	 }

	 current_task_ptr->task_timeout += timeout;
	 current_task_ptr->state			 = STATE_BLOCKED;
	 current_task_ptr->blocked_reason = BLOCKED_WAIT_SIGNAL;

	 enable_interrupts();

	 trigger_context_switch();

	 disable_interrupts();

	 if (current_task_ptr->blocked_reason != BLOCKED_NONE)
	 {
		  current_task_ptr->blocked_reason = BLOCKED_NONE;
		  enable_interrupts();

		  return FALSE;
	 }

	 memcpy(pdata, q_ptr->front, q_ptr->element_size);
	 s_move_queue_ptr(&q_ptr->front, queue_id);

	 enable_interrupts();
	 return TRUE;
}

uint8_t create_mutex(void)
{
	 disable_interrupts();

	 static uint8_t mutex_id = 0;

	 if (mutex_id > MAX_MUTEX)
	 {
		  return -1;
	 }

	 enable_interrupts();

	 return mutex_id++;
}

void lock_mutex(const uint8_t mutex_id)
{
	 disable_interrupts();

	 ST_MUTEX *mutex_ptr = &mutex_pool[mutex_id];

	 if (mutex_ptr->is_lock)
	 {
		  mutex_ptr->is_lock	 = FALSE;
		  mutex_ptr->owner_id = current_task_ptr->task_id;

		  enable_interrupts();
		  return;
	 }

	 mutex_ptr->waiting_task[current_task_ptr->task_id] = TRUE;
	 blocked_cur_task(BLOCKED_MUTEX_LOCK, 0);
	 s_delete_ready_task(current_task_ptr->task_id);

	 ST_Task *owner_task_ptr = &task_pool[mutex_ptr->owner_id];

	 if (owner_task_ptr->prio > current_task_ptr->prio)
	 {
		  /* Priority Inheritance */
		  if (owner_task_ptr->state == STATE_READY)
		  {
				s_delete_ready_task(owner_task_ptr->task_id);
				owner_task_ptr->prio = current_task_ptr->prio;
				s_insert_ready_task(owner_task_ptr->task_id);
		  }
		  else
		  {
				owner_task_ptr->prio = current_task_ptr->prio;
		  }
	 }

	 trigger_context_switch();
	 enable_interrupts();
}

void unlock_mutex(const uint8_t mutex_id)
{
	 disable_interrupts();

	 ST_MUTEX *mutex_ptr		= &mutex_pool[mutex_id];
	 uint8_t	  trigger_flag = FALSE;

	 if (mutex_ptr->owner_id != current_task_ptr->task_id)
	 {
		  enable_interrupts();
		  return;
	 }

	 mutex_ptr->is_lock	= TRUE;
	 mutex_ptr->owner_id = -1;

	 // Priority restore
	 if (current_task_ptr->prio != current_task_ptr->origin_prio)
	 {
		  s_delete_ready_task(current_task_ptr->task_id);
		  current_task_ptr->prio = current_task_ptr->origin_prio;
		  s_insert_ready_task(current_task_ptr->task_id);

		  trigger_flag = TRUE;
	 }

	 uint32_t highest_prio				  = PRIO_LOWEST + 1;
	 ST_Task *highest_waiting_task_ptr = nullptr;

	 for (int8_t i = 0; i < MAX_TASK; i++)
	 {
		  if (mutex_ptr->waiting_task[i])
		  {
				highest_waiting_task_ptr = &task_pool[mutex_ptr->waiting_task[i]];

				if (highest_waiting_task_ptr->prio < highest_prio)
				{
					 highest_prio = highest_waiting_task_ptr->prio;
				}
		  }
	 }

	 if (highest_waiting_task_ptr != nullptr)
	 {
		  mutex_ptr->is_lock														 = FALSE;
		  mutex_ptr->owner_id													 = highest_waiting_task_ptr->task_id;
		  mutex_ptr->waiting_task[highest_waiting_task_ptr->task_id] = FALSE;

		  highest_waiting_task_ptr->state			 = STATE_READY;
		  highest_waiting_task_ptr->blocked_reason = BLOCKED_NONE;

		  s_insert_ready_task(highest_waiting_task_ptr->task_id);

		  trigger_flag = TRUE;
	 }

	 if (trigger_flag)
	 {
		  trigger_context_switch();
	 }

	 enable_interrupts();
}
