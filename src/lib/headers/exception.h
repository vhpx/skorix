#ifndef EXCEPTION_H
#define EXCEPTION_H

static char exceptions[16][16] = {
    "SYNC_EL0",
    "IRQ_EL0",
    "FIQ_EL0",
    "ERROR_EL0",

    "SYNC_ELx",
    "IRQ_ELx",
    "FIQ_ELx",
    "ERROR_ELx",

    "SYNC_EL_64",
    "IRQ_EL_64",
    "FIQ_EL_64",
    "ERROR_EL_64",

    "SYNC_EL_32",
    "IRQ_EL_32",
    "FIQ_EL_32",
    "ERROR_EL_32"
};

// Assembly functions
void exception_init(void);
int get_el(void);

// C functions
void show_exception(unsigned int type, unsigned long esr, unsigned long address);

#endif
