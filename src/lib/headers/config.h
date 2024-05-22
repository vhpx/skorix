#include "constants.h"

#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef struct {
  char text_color[10];
  char background_color[10];
} OSConfig;

typedef struct {
  unsigned int ibrd;
  unsigned int fbrd;
} BaudRateConfig;

extern OSConfig OS_CONFIG;
extern BaudRateConfig BAUD_RATE_CONFIG;

void calculate_baud_rate_config(int baud_rate, BaudRateConfig *config);
int baud_rate_config_to_int(BaudRateConfig *config);

int uart_set_baudrate(int value, BaudRateConfig *config);
int uart_set_databits(int value, int *config);
int uart_set_stopbits(int value, int *config);
int uart_set_parity(int value, int *config);
int uart_set_flow_control(int value, int *config);

extern int BAUD_RATE_CONFIG_VALUE;
extern int DBIT_CONFIG;
extern int SBIT_CONFIG;
extern int PARITY_MODE_CONFIG;
extern int SEND_FLOW_CONFIG;

#endif
