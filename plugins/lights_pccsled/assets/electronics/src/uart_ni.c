#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart_ni.h"


void uart_init(uint32_t baudrate) {
    // set baud rate
    uint8_t sreg = SREG;

    switch(baudrate) {
    default:
    case 9600:
        UBRRL=103;
        UBRRH=0;
        break;
    case 14400:
        UBRRL=68;
        UBRRH=0;
        break;
    case 19200:
        UBRRL=51;
        UBRRH=0;
        break;
    case 57600:
        UBRRL=16;
        UBRRH=0;
        break;
    case 115200:
        UBRRL=8;
        UBRRH=0;
        break;
    case 250000:
        UBRRL=3;
        UBRRH=0;
        break;
    case 500000:
        UBRRL=1;
        UBRRH=0;
        break;
    case 1000000:
        UBRRL=0;
        UBRRH=0;
        break;

    }

    // UART Receiver und Transmitter anschalten
    // Data mode 8N1, asynchron
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

    // Flush Receive-Buffer (entfernen evtl. vorhandener ungültiger Werte)
    do {
        UDR;
    } while (UCSRA & (1 << RXC));

}



int uart_putchar( char c, FILE *stream ) {
    //if( c == '\n' )
    //    uart_putc( '\r');

    uart_putc(c);
    return 0;
}

int uart_getchar (FILE *stream) {
    return uart_getc_wait();
}

void uart_puts (const char *s) {
    do {
        uart_putc (*s);
    } while (*s++);
}

