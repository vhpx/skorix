#include "../headers/irq-entry.h"

.macro entry
    sub sp,  sp,  #STACK_FRAME_SIZE
    stp x0,  x1,  [sp, #16 * 0]
    stp x2,  x3,  [sp, #16 * 1]
    stp x4,  x5,  [sp, #16 * 2]
    stp x6,  x7,  [sp, #16 * 3]
    stp x8,  x9,  [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, x29, [sp, #16 * 14]
    str x30, [sp, #16 * 15]
.endm

.macro exit
    ldp x0,  x1,  [sp, #16 * 0]
    ldp x2,  x3,  [sp, #16 * 1]
    ldp x4,  x5,  [sp, #16 * 2]
    ldp x6,  x7,  [sp, #16 * 3]
    ldp x8,  x9,  [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    ldr x30, [sp, #16 * 15]
    add sp,  sp,  #STACK_FRAME_SIZE
    eret
.endm

.macro handle_invalid type
    entry
    mov x0, #\type
    mrs x1, esr_el1
    mrs x2, elr_el1
    b   hang
.endm

.macro vector label
.align 7
    b  \label
.endm

.align 11
.globl vectors
vectors:
    vector sync_el1t
    vector irq_el1t
    vector fiq_el1t
    vector error_el1t

    vector sync_el1h
    vector irq_el1h
    vector fiq_el1h
    vector error_el1h

    vector sync_el0_64
    vector irq_el0_64
    vector fiq_el0_64
    vector error_el0_64

    vector sync_el0_32
    vector irq_el0_32
    vector fiq_el0_32
    vector error_el0_32

sync_el1t:
    handle_invalid SYNC_EL1t

irq_el1t:
    handle_invalid IRQ_EL1t

fiq_el1t:
    handle_invalid FIQ_EL1t

error_el1t:
    handle_invalid ERROR_EL1t

sync_el1h:
    handle_invalid SYNC_EL1h

irq_el1h:
    entry
    bl handle_irq
    exit

fiq_el1h:
    handle_invalid FIQ_EL1h

error_el1h:
    handle_invalid ERROR_EL1h

sync_el0_64:
    handle_invalid SYNC_EL0_64

irq_el0_64:
    handle_invalid IRQ_EL0_64

fiq_el0_64:
    handle_invalid FIQ_EL0_64

error_el0_64:
    handle_invalid ERROR_EL0_64

sync_el0_32:
    handle_invalid SYNC_EL0_32

irq_el0_32:
    handle_invalid IRQ_EL0_32

fiq_el0_32:
    handle_invalid FIQ_EL0_32

error_el0_32:
    handle_invalid ERROR_EL0_32

.globl hang
hang: b hang