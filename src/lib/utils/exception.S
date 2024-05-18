// https://github.com/rockytriton/LLD/tree/main/rpi_bm/part9
// https://github.com/babbleberry/rpi4-osdev/tree/master/part13-interrupts

#define SYNC_EL0            0
#define IRQ_EL0             1
#define FIQ_EL0             2
#define ERROR_EL0           3

#define SYNC_ELx            4
#define IRQ_ELx             5
#define FIQ_ELx             6
#define ERROR_ELx           7

#define SYNC_EL_64          8
#define IRQ_EL_64           9
#define FIQ_EL_64           10
#define ERROR_EL_64         11

#define SYNC_EL_32          12
#define IRQ_EL_32           13
#define FIQ_EL_32           14
#define ERROR_EL_32         15

#define STACK_FRAME_SIZE    256

// Save processor state
.macro save_state
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

// Restore processor state
.macro restore_state
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

// Hang the processor by default on most exceptions
.macro handle_exception_default type
    save_state
    mov x0, #\type
    mrs x1, esr_el2
    mrs x2, elr_el2
    bl  show_exception
    b   hang
.endm

.macro exception label
.align 7
    b  \label
.endm

.align 11
exceptions:
    exception sync_el0
    exception irq_el0
    exception fiq_el0
    exception error_el0

    exception sync_elx
    exception irq_elx
    exception fiq_elx
    exception error_elx

    exception sync_el_64
    exception irq_el_64
    exception fiq_el_64
    exception error_el_64

    exception sync_el_32
    exception irq_el_32
    exception fiq_el_32
    exception error_el_32

sync_el0:
    handle_exception_default SYNC_EL0

irq_el0:
    handle_exception_default IRQ_EL0

fiq_el0:
    handle_exception_default FIQ_EL0

error_el0:
    handle_exception_default ERROR_EL0

sync_elx:
    handle_exception_default SYNC_ELx

irq_elx:
    save_state
    bl handle_irq_elx
    restore_state

fiq_elx:
    handle_exception_default FIQ_ELx

error_elx:
    handle_exception_default ERROR_ELx

sync_el_64:
    handle_exception_default SYNC_EL_64

irq_el_64:
    handle_exception_default IRQ_EL_64

fiq_el_64:
    handle_exception_default FIQ_EL_64

error_el_64:
    handle_exception_default ERROR_EL_64

sync_el_32:
    handle_exception_default SYNC_EL_32

irq_el_32:
    handle_exception_default IRQ_EL_32

fiq_el_32:
    handle_exception_default FIQ_EL_32

error_el_32:
    handle_exception_default ERROR_EL_32

hang: b hang

// Get current exception level (EL2 by default)
.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret

// Write exception table to EL2 Vector Base Address Register
.globl exception_init
exception_init:
    adr x0, exceptions
    msr vbar_el2, x0
    ret
