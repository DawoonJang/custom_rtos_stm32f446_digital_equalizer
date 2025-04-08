	.syntax unified
	.thumb

	.text

	.align 4

    .extern current_task_ptr
    .extern switching_task

	.global PendSV_Handler
	.type 	PendSV_Handler, %function
PendSV_Handler:
    mrs     r0, psp

    stmdb   r0!, {r4-r11}

    ldr     r3, =current_task_ptr
    ldr     r2, [r3]
    str     r0, [r2]

    push    {r3, lr}
    cpsid   i

    bl      switching_task

    cpsie   i
    pop     {r3, lr}

    ldr     r2, [r3] 
    ldr     r0, [r2]

    ldmia   r0!, {r4-r11}

    msr     psp, r0
    bx      lr