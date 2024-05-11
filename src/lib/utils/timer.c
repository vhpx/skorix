#include "../headers/timer.h"
#include "../headers/uart0.h"

unsigned int timer1 = 0;

void timer_init(void) {
    timer1 = TIMER_CLO + TIMER_CLOCK;
    TIMER_C1 = timer1;
}

void handle_timer(void) {
    timer1 += TIMER_CLOCK;
    TIMER_C1 = timer1;
    TIMER_CS |= TIMER_CS_M1;
    legacy_uart_puts("Timer interrupt\n");
}