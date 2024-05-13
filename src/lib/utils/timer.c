#include "../headers/timer.h"
#include "../headers/uart0.h"

unsigned int timer1_cmp = 0;
unsigned int timer1_period = TIMER_CLOCK;

void sys_timer1_init(void) {
    timer1_cmp = TIMER_CLO + timer1_period;
    TIMER_C1 = timer1_cmp;
    TIMER_CS |= TIMER_CS_M1;
}

void handle_sys_timer1(void) {
    timer1_cmp += timer1_period;
    TIMER_C1 = timer1_cmp;
    TIMER_CS |= TIMER_CS_M1;

    // Function here
    // uart_puts("Timer 1 interrupt\n");
}
