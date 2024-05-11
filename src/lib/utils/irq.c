#include "../headers/entry.h"
#include "../headers/irq.h"
#include "../headers/timer.h"
#include "../headers/uart0.h"

void timer_irq_enable(void) {
#ifdef RPI3
    ENABLE_IRQS_1 |= SYS_TIMER_1_IRQ;
#else
    IRQ0_SET_EN_0 |= SYS_TIMER_1_IRQ;
#endif
}

void timer_irq_disable(void) {
#ifdef RPI3
    DISABLE_IRQS_1 |= SYS_TIMER_1_IRQ;
#else
    IRQ0_CLR_EN_0 |= SYS_TIMER_1_IRQ;
#endif
}

void handle_irq(void) {
    unsigned int irq;

#ifdef RPI3
    irq = IRQ_PENDING_1;
#else
    irq = IRQ0_PENDING0;
#endif

    while (irq) {
        if (irq & SYS_TIMER_1_IRQ) {
            handle_timer();
            irq &= ~SYS_TIMER_1_IRQ;
        }
    }
}

void please_show_something(void) {
	legacy_uart_puts("Please show something\n");
}