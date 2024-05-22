#include "../headers/interrupt.h"
#include "../headers/timer.h"

void sys_timer1_irq_enable(void) {
#ifdef RPI3
    IRQ_ENABLE_1 |= SYS_TIMER_1_IRQ;
#else
    IRQ0_SET_EN_0 |= SYS_TIMER_1_IRQ;
#endif
}

void sys_timer1_irq_disable(void) {
#ifdef RPI3
    IRQ_DISABLE_1 |= SYS_TIMER_1_IRQ;
#else
    IRQ0_CLR_EN_0 |= SYS_TIMER_1_IRQ;
#endif
}

void sys_timer3_irq_enable(void) {
#ifdef RPI3
    IRQ_ENABLE_1 |= SYS_TIMER_3_IRQ;
#else
    IRQ0_SET_EN_0 |= SYS_TIMER_3_IRQ;
#endif
}

void sys_timer3_irq_disable(void) {
#ifdef RPI3
    IRQ_DISABLE_1 |= SYS_TIMER_3_IRQ;
#else
    IRQ0_CLR_EN_0 |= SYS_TIMER_3_IRQ;
#endif
}

void handle_irq_elx(void) {
    unsigned int irq_pending;

#ifdef RPI3
    irq_pending = IRQ_PENDING_1;
#else
    irq_pending = IRQ0_PENDING0;
#endif

    if (irq_pending & SYS_TIMER_1_IRQ) {
        handle_sys_timer1();
        irq_pending &= ~SYS_TIMER_1_IRQ;
    }

    if (irq_pending & SYS_TIMER_3_IRQ) {
        handle_sys_timer3();
        irq_pending &= ~SYS_TIMER_3_IRQ;
    }
}
