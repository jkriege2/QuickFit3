#ifndef _UART_H_
#define _UART_H_

#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>


/* initialize the UART for data transmission
 *     baudrate = baud rate in bits/second
 *     databits = [5|6|7|8|9] number of databits to transmit (default: 8)
 *     stopbits = [1|2] number of stop bits (default: 1)
 *     parity = [0 (none) | 1 (even) | 2 (dd)]
 *
 * NOTE: the precompiler symbo, F_CPU must be defined with the quarz frequency in Hz !!!
 */
#define UART_NOPARITY 0
#define UART_EVENPARITY 1
#define UART_ODDPARITY 2

extern void uart_init(uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity);



static inline int
uart_putc (const uint8_t c) {
    // Warten, bis UDR bereit ist für einen neuen Wert
    while (!(UCSRA & (1 << UDRE)))
        ;

    // UDR Schreiben startet die Übertragung
    UDR = c;

    return 1;
}


//extern int uart_putc (const uint8_t);
extern uint8_t uart_getc_wait(void);
extern int     uart_getc_nowait(void);

static inline void uart_flush(void) {
    while (UCSRB & (1 << UDRIE));
}



int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);


// Einen 0-terminierten String übertragen.
void uart_puts (const char *s);
void uart_puts_P (PGM_P s);

#endif /* _UART_H_ */
