#include "../headers/exception.h"
#include "../headers/uart0.h"

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
