#include "../headers/exception.h"
#include "../headers/uart0.h"

char* exceptions[] = {
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

void show_exception(unsigned int type, unsigned long esr, unsigned long address) {
    uart_puts("\n\nSomething went wrong...\n");
    uart_puts("Exception: ");
    uart_puts(exceptions[type]);
    uart_puts(" (");
    uart_hex(esr);
    uart_puts(")\n");
    uart_puts("Address: ");
    uart_hex(address);
    uart_puts("\n");
}
