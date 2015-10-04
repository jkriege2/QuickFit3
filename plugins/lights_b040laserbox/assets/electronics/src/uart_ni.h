#ifndef _UART_H_
#define _UART_H_

#include <avr/io.h>
#include <stdio.h>
extern void uart_init(uint32_t baudrate);


static inline int
uart_putc (const uint8_t c) {
    // Warten, bis UDR bereit ist für einen neuen Wert
    while (!(UCSRA & (1 << UDRE)))
        ;

    // UDR Schreiben startet die Übertragung
    UDR = c;

    return 1;
}

static inline uint8_t
uart_getc_wait() {
    // Warten, bis etwas empfangen wird
    while (!(UCSRA & (1 << RXC)))
        ;

    // Das empfangene Zeichen zurückliefern
    return UDR;
}

static inline int
uart_getc_nowait() {
    // Liefer das empfangene Zeichen, falls etwas empfangen wurde; -1 sonst
    return (UCSRA & (1 << RXC)) ? (int) UDR : -1;
}

int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);


// Einen 0-terminierten String übertragen.
void uart_puts (const char *s);

#endif /* _UART_H_ */
