#include "../headers/timer.h"
#include "../headers/uart0.h"

void timer_init(void) {
    TIMER_C1 = TIMER_CLO + TIMER_CLOCK;
}

void handle_timer(void) {
    TIMER_C1 += TIMER_CLOCK;
    TIMER_CS |= TIMER_CS_M1;
    legacy_uart_puts("Timer interrupt\n");
}