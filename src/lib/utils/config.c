#include "../headers/config.h"
#include "../headers/uart0.h"

#include "../headers/color.h"
#include "../headers/constants.h"
#include "../headers/string.h"

OSConfig OS_CONFIG = {
    .text_color = {'\0'},
    .background_color = {'\0'},
};

void print_decimal(int num) {
  if (num < 0) {
    uart_puts("-");
    num = -num;
  }
  if (num / 10) {
    print_decimal(num / 10);
  }

  char str[MAX_CMD_SIZE];
  int2str((num % 10) + '0', str);
  uart_puts(str);
}

BaudRateConfig BAUD_RATE_CONFIG;

int BAUD_RATE_CONFIG_VALUE = 115200; // Default Baud Rate
int DBIT_CONFIG = 8;                 // 8 Data Bits (options: 5, 6, 7, 8)
int SBIT_CONFIG = 1;                 // 1 = One Stop Bit, 2 = Two Stop Bits
int PARITY_MODE_CONFIG = 0; // 0 = No Parity, 1 = Odd Parity, 2 = Even Parity
int SEND_FLOW_CONFIG = 0;   // 0 = No Flow Control, 1 = Flow Control (CTS/RTS)

void calculate_baud_rate_config(int baud_rate, BaudRateConfig *config) {
  // Calculate Divider
  float divider = (float)CLK_SPEED / (16.0f * baud_rate);

  // Calculate integer and fractional parts of Divider
  int integerPart = (int)divider;
  float fractionalPart = divider - integerPart;

  // Calculate the integer part of the baud rate divisor
  config->ibrd = integerPart;

  // Calculate the fractional part of the baud rate divisor
  config->fbrd = (int)(fractionalPart * 64 + 0.5);
}

int baud_rate_config_to_int(BaudRateConfig *config) {
  return (int)(CLK_SPEED / (16.0f * (config->ibrd + config->fbrd / 64.0f)));
}

int uart_set_baudrate(int baudrate, BaudRateConfig *config) {
  switch (baudrate) {
  case 300:
  case 600:
  case 1200:
  case 2400:
  case 4800:
  case 9600:
  case 14400:
  case 19200:
  case 38400:
  case 57600:
  case 115200:
  case 230400:
  case 460800:
  case 921600:
    calculate_baud_rate_config(baudrate, config);
    BAUD_RATE_CONFIG_VALUE = baudrate;
    return 0;

  default:
    uart_puts("\n\nInvalid baud rate value. Please enter a valid baud "
              "rate value.\n\n");
    uart_puts(
        "Your baud rate must be one of the following values: 300, 1200, 2400, "
        "4800, 9600, "
        "19200, 38400, 57600, 115200, 230400, 460800, 921600.\n\n");

    config->ibrd = BAUD_RATE_CONFIG.ibrd;
    config->fbrd = BAUD_RATE_CONFIG.fbrd;
    break;
  }

  return 1;
}

int uart_set_databits(int value, int *config) {
  switch (value) {
  case 5:
  case 6:
  case 7:
  case 8:
    *config = value;
    return 0;

  default:
    uart_puts("\n\nInvalid data bits value. Please enter a valid data "
              "bits value.\n\n");
    uart_puts(
        "Your data bits value must be one of the following values: 5, 6, 7, "
        "8.\n\n");

    *config = DBIT_CONFIG;
    break;
  }

  return 1;
}

int uart_set_stopbits(int value, int *config) {
  switch (value) {
  case 1:
  case 2:
    *config = value;
    return 0;

  default:
    uart_puts("\n\nInvalid stop bits value. Please enter a valid stop "
              "bits value.\n\n");
    uart_puts(
        "Your stop bits value must be one of the following values: 1, 2.\n\n");

    *config = SBIT_CONFIG;
    break;
  }

  return 1;
}

int uart_set_parity(int value, int *config) {
  switch (value) {
  case 0:
  case 1:
  case 2:
    *config = value;
    return 0;

  default:
    uart_puts("\n\nInvalid parity mode value. Please enter a valid "
              "parity mode value.\n\n");
    uart_puts(
        "Your parity mode value must be one of the following values: 0 (No "
        "Parity), 1 (Odd Parity), 2 (Even Parity).\n\n");

    *config = PARITY_MODE_CONFIG;
    break;
  }

  return 1;
}

int uart_set_flow_control(int value, int *config) {
  switch (value) {
  case 0:
  case 1:
    *config = value;
    return 0;

  default:
    uart_puts("\n\nInvalid flow control value. Please enter a valid "
              "flow control value.\n\n");
    uart_puts(
        "Your flow control value must be one of the following values: 0 (No "
        "Flow Control), 1 (CTS/RTS).\n\n");

    *config = SEND_FLOW_CONFIG;
    break;
  }

  return 1;
}