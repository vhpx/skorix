#ifndef IRQ_H
#define IRQ_H

#include "gpio.h"

void irq_init(void);
void irq_enable(void);
void irq_disable(void);

#endif