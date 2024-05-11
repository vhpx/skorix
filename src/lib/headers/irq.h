#ifndef IRQ_H
#define IRQ_H

#include "gpio.h"

#define INTERRUPT_BASE     (MMIO_BASE + 0xB000)

// RPI3
#define IRQ_BASIC_PENDING  (*(volatile unsigned int *)(INTERRUPT_BASE + 0x200))
#define IRQ_PENDING_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x204))
#define IRQ_PENDING_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x208))
#define FIQ_CONTROL        (*(volatile unsigned int *)(INTERRUPT_BASE + 0x20C))
#define ENABLE_IRQS_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x210))
#define ENABLE_IRQS_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x214))
#define ENABLE_BASIC_IRQS  (*(volatile unsigned int *)(INTERRUPT_BASE + 0x218))
#define DISABLE_IRQS_1     (*(volatile unsigned int *)(INTERRUPT_BASE + 0x21C))
#define DISABLE_IRQS_2     (*(volatile unsigned int *)(INTERRUPT_BASE + 0x220))
#define DISABLE_BASIC_IRQS (*(volatile unsigned int *)(INTERRUPT_BASE + 0x224))

// RPI4
#define IRQ0_PENDING0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x200))
#define IRQ0_PENDING1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x204))
#define IRQ0_PENDING2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x208))
#define IRQ0_SET_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x210))
#define IRQ0_SET_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x214))
#define IRQ0_SET_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x218))
#define IRQ0_CLR_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x220))
#define IRQ0_CLR_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x224))
#define IRQ0_CLR_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x228))
#define IRQ0_STATUS0       (*(volatile unsigned int *)(INTERRUPT_BASE + 0x230))
#define IRQ0_STATUS1       (*(volatile unsigned int *)(INTERRUPT_BASE + 0x234))
#define IRQ0_STATUS2       (*(volatile unsigned int *)(INTERRUPT_BASE + 0x238))
#define IRQ1_PENDING0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x240))
#define IRQ1_PENDING1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x244))
#define IRQ1_PENDING2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x248))
#define IRQ1_SET_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x250))
#define IRQ1_SET_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x254))
#define IRQ1_SET_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x258))
#define IRQ1_CLR_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x260))
#define IRQ1_CLR_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x264))
#define IRQ1_CLR_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x268))
#define IRQ2_PENDING0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x280))
#define IRQ2_PENDING1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x284))
#define IRQ2_PENDING2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x288))
#define IRQ2_SET_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x290))
#define IRQ2_SET_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x294))
#define IRQ2_SET_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x298))
#define IRQ2_CLR_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2A0))
#define IRQ2_CLR_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2A4))
#define IRQ2_CLR_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2A8))
#define IRQ3_PENDING0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2C0))
#define IRQ3_PENDING1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2C4))
#define IRQ3_PENDING2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2C8))
#define IRQ3_SET_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2D0))
#define IRQ3_SET_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2D4))
#define IRQ3_SET_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2D8))
#define IRQ3_CLR_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2E0))
#define IRQ3_CLR_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2E4))
#define IRQ3_CLR_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x2E8))
#define FIQ0_PENDING0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x300))
#define FIQ0_PENDING1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x304))
#define FIQ0_PENDING2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x308))
#define FIQ0_SET_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x310))
#define FIQ0_SET_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x314))
#define FIQ0_SET_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x318))
#define FIQ0_CLR_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x320))
#define FIQ0_CLR_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x324))
#define FIQ0_CLR_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x328))
#define FIQ1_PENDING0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x340))
#define FIQ1_PENDING1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x344))
#define FIQ1_PENDING2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x348))
#define FIQ1_SET_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x350))
#define FIQ1_SET_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x354))
#define FIQ1_SET_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x358))
#define FIQ1_CLR_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x360))
#define FIQ1_CLR_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x364))
#define FIQ1_CLR_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x368))
#define FIQ2_PENDING0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x380))
#define FIQ2_PENDING1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x384))
#define FIQ2_PENDING2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x388))
#define FIQ2_SET_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x390))
#define FIQ2_SET_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x394))
#define FIQ2_SET_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x398))
#define FIQ2_CLR_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3A0))
#define FIQ2_CLR_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3A4))
#define FIQ2_CLR_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3A8))
#define FIQ3_PENDING0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3C0))
#define FIQ3_PENDING1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3C4))
#define FIQ3_PENDING2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3C8))
#define FIQ3_SET_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3D0))
#define FIQ3_SET_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3D4))
#define FIQ3_SET_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3D8))
#define FIQ3_CLR_EN_0      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3E0))
#define FIQ3_CLR_EN_1      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3E4))
#define FIQ3_CLR_EN_2      (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3E8))
#define SWIRQ_SET          (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3F0))
#define SWIRQ_CLEAR        (*(volatile unsigned int *)(INTERRUPT_BASE + 0x3F4))

#define SYS_TIMER_0_IRQ    (1 << 0)
#define SYS_TIMER_1_IRQ    (1 << 1)
#define SYS_TIMER_2_IRQ    (1 << 2)
#define SYS_TIMER_3_IRQ    (1 << 3)

void irq_init(void);
void irq_enable(void);
void irq_disable(void);
void irq_wait(void);

void timer_irq_enable(void);
void handle_irq(void);

void please_show_something(void);

#endif