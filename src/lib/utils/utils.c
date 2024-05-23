#include "../headers/uart0.h"

/* New function for Uart: Check and return if no new character, don't wait */

#if 1 // UART0
unsigned int uart_isReadByteReady() { return (!(UART0_FR & UART0_FR_RXFE)); }

#else // UART1
unsigned int uart_isReadByteReady() { return (AUX_MU_LSR & 0x01); }
#endif

unsigned char getUart() {
  unsigned char ch = 0;
  if (uart_isReadByteReady())
    ch = uart_getc();
  return ch;
}

/* Functions to delay, set/wait timer */

void wait_msec(unsigned int n) {
  register unsigned long f, t, r, expiredTime;

  // Get the current counter frequency (Hz)
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));

  // Read the current counter value
  asm volatile("mrs %0, cntpct_el0" : "=r"(t));

  // Calculate expire value for counter
  expiredTime = t + ((f / 1000) * n) / 1000;
  do {
    asm volatile("mrs %0, cntpct_el0" : "=r"(r));
  } while (r < expiredTime);
}

void set_wait_timer(int set, unsigned int msVal) {
  static unsigned long expiredTime = 0; // declare static to keep value
  register unsigned long r, f, t;

  if (set) { /* SET TIMER */
    // Get the current counter frequency (Hz)
    asm volatile("mrs %0, cntfrq_el0" : "=r"(f));

    // Read the current counter
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));

    // Calculate expired time:
    expiredTime = t + ((f / 1000) * msVal) / 1000;
  } else { /* WAIT FOR TIMER TO EXPIRE */
    do {
      asm volatile("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < expiredTime);
  }
}
