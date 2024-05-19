/* Raspberry Pi's peripheral physical address (MMIO_BASE) is 0xFE000000 in RPI4,
0x3F000000 in RPI3
--> Select correct option to set the value properly
*/

//#define RPI3 // enable when using RPI3 (QEMU emulation/ real board)

#ifdef RPI3 // RPI3
#define MMIO_BASE 0x3F000000
#else // RPI4
#define MMIO_BASE 0xFE000000
#endif

// Define GPIO Registers based on their addresses
#define GPFSEL0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200000))
#define GPFSEL1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200004))
#define GPFSEL2 (*(volatile unsigned int *)(MMIO_BASE + 0x00200008))
#define GPFSEL3 (*(volatile unsigned int *)(MMIO_BASE + 0x0020000C))
#define GPFSEL4 (*(volatile unsigned int *)(MMIO_BASE + 0x00200010))
#define GPFSEL5 (*(volatile unsigned int *)(MMIO_BASE + 0x00200014))
#define GPSET0 (*(volatile unsigned int *)(MMIO_BASE + 0x0020001C))
#define GPSET1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200020))
#define GPCLR0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200028))
#define GPLEV0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200034))
#define GPLEV1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200038))
#define GPEDS0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200040))
#define GPEDS1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200044))
#define GPHEN0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200064))
#define GPHEN1 (*(volatile unsigned int *)(MMIO_BASE + 0x00200068))

// RBP3 only
#define GPPUD (*(volatile unsigned int *)(MMIO_BASE + 0x00200094))
#define GPPUDCLK0 (*(volatile unsigned int *)(MMIO_BASE + 0x00200098))
#define GPPUDCLK1 (*(volatile unsigned int *)(MMIO_BASE + 0x0020009C))

// RBP4 only
#define GPIO_PUP_PDN_CNTRL_REG0                                                \
  (*(volatile unsigned int *)(MMIO_BASE + 0x002000E4))
#define GPIO_PUP_PDN_CNTRL_REG1                                                \
  (*(volatile unsigned int *)(MMIO_BASE + 0x002000E8))
#define GPIO_PUP_PDN_CNTRL_REG2                                                \
  (*(volatile unsigned int *)(MMIO_BASE + 0x002000EC))
#define GPIO_PUP_PDN_CNTRL_REG3                                                \
  (*(volatile unsigned int *)(MMIO_BASE + 0x002000F0))
