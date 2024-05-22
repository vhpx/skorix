#include "../headers/timer.h"
#include "../headers/gpio.h"
#include "../games/unrob/game.h"
#include "../headers/cli.h"

unsigned int timer1_cmp = 0;
unsigned int timer1_period = TIMER_CLOCK / 2;
unsigned int timer3_cmp = 0;
unsigned int timer3_period = TIMER_CLOCK;

void sys_timer1_init(void) {
    // Set GPIO3 as output
    GPFSEL0 |= 1 << 9;

    timer1_cmp = TIMER_CLO + timer1_period;
    TIMER_C1 = timer1_cmp;
    TIMER_CS |= TIMER_CS_M1;
}

void handle_sys_timer1(void) {
    timer1_cmp += timer1_period;
    TIMER_C1 = timer1_cmp;
    TIMER_CS |= TIMER_CS_M1;

    // Turn GPIO3 on and off
    if (GPLEV0 & (1 << 3)) {
        GPCLR0 |= 1 << 3;
    } else {
        GPSET0 |= 1 << 3;
    }
}

void sys_timer3_init(void) {
    timer3_cmp = TIMER_CLO + timer3_period;
    TIMER_C3 = timer3_cmp;
    TIMER_CS |= TIMER_CS_M3;
}

void handle_sys_timer3(void) {
    timer3_cmp += timer3_period;
    TIMER_C3 = timer3_cmp;
    TIMER_CS |= TIMER_CS_M3;

    if (is_mode_game) {
        countdown();
    }
}
