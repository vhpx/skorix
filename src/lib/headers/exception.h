#ifndef EXCEPTION_H
#define EXCEPTION_H

// Assembly functions
void exception_init(void);
int get_el(void);

// C functions
void show_exception(unsigned int type, unsigned long esr, unsigned long address);

#endif
