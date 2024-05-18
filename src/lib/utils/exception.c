#include "../headers/exception.h"
#include "../headers/uart0.h"

void show_exception(unsigned int type, unsigned long esr, unsigned long far, unsigned long elr) {
    uart_puts("\n\nSomething went wrong...\n");
    uart_puts("Exception type: ");
    uart_puts(exceptions[type]);
    uart_puts("\n");
    uart_puts("ESR: ");
    uart_hex(esr);
    uart_puts("\n");
    uart_puts("FAR: ");
    uart_hex(far);
    uart_puts("\n");
    uart_puts("ELR: ");
    uart_hex(elr);
    uart_puts("\n");
}
