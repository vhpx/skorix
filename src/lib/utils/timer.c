#include "../headers/timer.h"
#include "../games/unrob/game.h"
#include "../headers/cli.h"
#include "../headers/gpio.h"

unsigned int timer1_cmp = 0;
unsigned int timer1_period = TIMER_CLOCK / 5;
unsigned int timer3_cmp = 0;
unsigned int timer3_period = TIMER_CLOCK / 5;

// Initialize system timer 1
void sys_timer1_init(void) {
  // Set GPIO3 as output
  GPFSEL0 |= 1 << 9;

  // Set the compare value for timer 1
  // This determines when the timer will trigger an interrupt
  timer1_cmp = TIMER_CLO + timer1_period;
  TIMER_C1 = timer1_cmp;

  // Enable the interrupt for timer 1
  TIMER_CS |= TIMER_CS_M1;
}

// Handle the interrupt for system timer 1
void handle_sys_timer1(void) {
  // Update the compare value for the next interrupt
  timer1_cmp += timer1_period;
  TIMER_C1 = timer1_cmp;

  // Acknowledge the interrupt
  TIMER_CS |= TIMER_CS_M1;

  // Toggle GPIO3
  // This will turn an LED or similar device on and off
  if (GPLEV0 & (1 << 3)) {
    GPCLR0 |= 1 << 3;
  } else {
    GPSET0 |= 1 << 3;
  }
}

// Initialize system timer 3
void sys_timer3_init(void) {
  // Set the compare value for timer 3
  // This determines when the timer will trigger an interrupt
  timer3_cmp = TIMER_CLO + timer3_period;
  TIMER_C3 = timer3_cmp;

  // Enable the interrupt for timer 3
  TIMER_CS |= TIMER_CS_M3;
}

// Handle the interrupt for system timer 3
void handle_sys_timer3(void) {
  // Update the compare value for the next interrupt
  timer3_cmp += timer3_period;
  TIMER_C3 = timer3_cmp;

  // Acknowledge the interrupt
  TIMER_CS |= TIMER_CS_M3;

  // If the current mode is GAME, call the countdown function
  // This is used to update our game's timer
  if (mode == GAME) {
    countdown();
  }
}
