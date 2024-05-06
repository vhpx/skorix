#include "../../lib/headers/uart0.h"
#include "../../lib/headers/color.h"
#include "../../lib/headers/config.h"
#include "../../lib/headers/string.h"

void delay(int cycles) {
  while (cycles--) {
    __asm volatile("nop");
  }
}

void setup_gpio() {
  unsigned int r;

  // Configure GPIO 14 as TXD and GPIO 15 as RXD
  r = GPFSEL1;
  r &= ~((7 << 12) | (7 << 15)); // clear bits 17-12 (FSEL15, FSEL14)
  r |=
      (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
  GPFSEL1 = r;

  // Configure GPIO 16 as CTS and GPIO 17 as RTS
  if (SEND_FLOW_CONFIG == 1) {
    r = GPFSEL1;
    r &= ~((7 << 18) | (7 << 21)); // clear bits 21-18 (FSEL17, FSEL16)
    r |= (7 << 18) | (7 << 21);    // Set value 0b111 (select ALT3: CTS0/RTS0)
    GPFSEL1 = r;
  }

  // Enable pull-up on GPIO 14, 15
  GPPUD = 0; // No pull up/down control
  delay(150);
  GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
  delay(150);
  GPPUDCLK0 = 0; // flush GPIO setup
}

void setup_uart() {
  /* Mask all interrupts */
  UART0_IMSC = 0;

  /* Clear pending interrupts */
  UART0_ICR = 0x7FF;

  // Calculate baud rate
  BaudRateConfig temp_baud_rate_config;
  calculate_baud_rate_config(BAUD_RATE_CONFIG_VALUE, &temp_baud_rate_config);

  // Update the global baud rate config
  BAUD_RATE_CONFIG.fbrd = temp_baud_rate_config.fbrd;
  BAUD_RATE_CONFIG.ibrd = temp_baud_rate_config.ibrd;

  /* Set integer & fractional part of Baud rate */
  UART0_IBRD = BAUD_RATE_CONFIG.ibrd;
  UART0_FBRD = BAUD_RATE_CONFIG.fbrd;

  /* Set up the Line Control Register */
  // Clean previous settings
  UART0_LCRH = 0;

  // Set data bits
  if (DBIT_CONFIG == 5) {
    UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_5BIT;
  } else if (DBIT_CONFIG == 6) {
    UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_6BIT;
  } else if (DBIT_CONFIG == 7) {
    UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_7BIT;
  } else if (DBIT_CONFIG == 8) {
    UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;
  }

  // Set stop bits
  if (SBIT_CONFIG == 1) {
    UART0_LCRH &= ~UART0_LCRH_STP2;
  } else if (SBIT_CONFIG == 2) {
    UART0_LCRH |= UART0_LCRH_STP2;
  }

  // Set parity
  if (PARITY_MODE_CONFIG == 0) {
    UART0_LCRH &= ~UART0_LCRH_PEN;
  } else if (PARITY_MODE_CONFIG == 1) {
    UART0_LCRH |= UART0_LCRH_PEN;
    UART0_LCRH &= ~UART0_LCRH_EPS;
  } else if (PARITY_MODE_CONFIG == 2) {
    UART0_LCRH |= UART0_LCRH_PEN;
    UART0_LCRH |= UART0_LCRH_EPS;
  }

  /* Enable UART0, receive, and transmit */
  UART0_CR = UART0_CR_UARTEN | UART0_CR_TXE | UART0_CR_RXE;

  // check for handshake mode
  if (SEND_FLOW_CONFIG == 1) {
    UART0_CR |= UART0_CR_RTSEN | UART0_CR_CTSEN; // enable RTS, CTS
  }
}

void uart_init(int restart) {
  // Disable UART0
  UART0_CR = 0;

  // Setup GPIO for UART
  setup_gpio();

  // Setup UART
  setup_uart();

  // Display restart message
  if (restart == 1) {
    legacy_uart_puts("UART0 has been restarted.\n");
    legacy_uart_puts("\n");
  }
}

/**
 * Receive a character
 */
char uart_getc() {
  char c = 0;

  /* Check Flags Register */
  /* Wait until Receiver is not empty
   * (at least one byte data in receive fifo)*/
  do {
    __asm volatile("nop");
  } while (UART0_FR & UART0_FR_RXFE);

  /* read it and return */
  c = (unsigned char)(UART0_DR);

  /* convert carriage return to newline */
  return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(char *s) {
  // If the text color is set, set it
  if (strlen(OS_CONFIG.text_color) || OS_CONFIG.text_color != (char *)0)
    legacy_uart_puts(OS_CONFIG.text_color);

  // If the background color is set, set it
  if (strlen(OS_CONFIG.background_color) ||
      OS_CONFIG.background_color != (char *)0)
    legacy_uart_puts(OS_CONFIG.background_color);

  legacy_uart_puts(s);
}

/**
 * Send a character
 */
void uart_sendc(char c) {
  /* Check Flags Register */
  /* And wait until transmitter is not full */
  do {
    __asm volatile("nop");
  } while (UART0_FR & UART0_FR_TXFF);

  /* Write our data byte out to the data register */
  UART0_DR = c;
}

void legacy_uart_puts(char *s) {
  while (*s) {
    /* convert newline to carriage return + newline */
    if (*s == '\n')
      uart_sendc('\r');
    uart_sendc(*s++);
  }
}

/**
* Display a value in hexadecimal format
*/
void uart_hex(unsigned int num) {
    uart_puts("0x");
    for (int pos = 28; pos >= 0; pos = pos - 4) {
        // Get highest 4-bit nibble
        char digit = (num >> pos) & 0xF;
        /* Convert to ASCII code */
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        digit += (digit > 9) ? (-10 + 'A') : '0';
        uart_sendc(digit);
    }
}

/**
* Display a value in decimal format
*/
void uart_dec(int num) {
    //A string to store the digit characters
    char str[33] = "";
    //Calculate the number of digits
    int len = 1;
    int temp = num;
    while (temp >= 10){
        len++;
        temp = temp / 10;
    }
    
    //Store into the string and print out
    for (int i = 0; i < len; i++){
        int digit = num % 10; //get last digit
        num = num / 10; //remove last digit from the number
        str[len - (i + 1)] = digit + '0';
    }
    str[len] = '\0';
    uart_puts(str);
}
