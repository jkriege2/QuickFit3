//
// LaserBox - a µController controller for Cobolt DPSS Lasers
//  (C) 2011-2013 Jan W. Krieger <j.krieger@dkfz.de, jan@jkrieger.de>
//
// This file is part of LaserBox.
//
// LaserBox is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaserBox is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Copyright Header. If not, see <http://www.gnu.org/licenses/>.
//


#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "fifo.h" // erklärt im Artikel "FIFO mit avr-gcc"




#define REG_UCSRA UCSR0A
#define REG_UCSRB UCSR0B
#define REG_UCSRC UCSR0C
#define REG_UDR UDR0
#define REG_UBRRL UBRR0L
#define REG_UBRRH UBRR0H
#define REG_UBRR UBRR0

#define BIT_UDRE UDRE0
#define BIT_UDRIE UDRIE0
#define BIT_RXEN RXEN0
#define BIT_TXEN TXEN0
#define BIT_RXCIE RXCIE0
#define BIT_RUPM1 RUPM01
#define BIT_UPM0 UPM00
#define BIT_UPM1 UPM01
#define BIT_URSEL URSEL0
#define BIT_UCSZ0 UCSZ00
#define BIT_UCSZ1 UCSZ01
#define BIT_UCSZ2 UCSZ02
#define BIT_USBS USBS0
#define BIT_U2X U2X0
#define BIT_RXC RXC0
#define BIT_TXC TXC0

#define SIG_UART_RECV USART0_RX_vect
#define SIG_UART_DATA USART0_UDRE_vect


// FIFO-Objekte und Puffer für die Ein- und Ausgabe

#define BUFSIZE_IN  32
uint8_t inbuf[BUFSIZE_IN];
fifo_t infifo;

#define BUFSIZE_OUT 1
uint8_t outbuf[BUFSIZE_OUT];
fifo_t outfifo;


int uart_putc (const uint8_t c) {
    // Warten, bis UDR bereit ist für einen neuen Wert
    while (!(REG_UCSRA & (1 << BIT_UDRE)))
        ;

    // UDR Schreiben startet die Übertragung
    REG_UDR = c;

    return 1;
}

void uart_flush(void) {
    while (REG_UCSRB & (1 << BIT_UDRIE));
    fifo_init (&infifo,   inbuf, BUFSIZE_IN);
}

void uart_init(uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {
    // set baud rate
    uint8_t sreg = SREG;

    uint8_t U2X_val=0;
    uint16_t ubrv;


    switch(baudrate) {
      #if defined (__AVR_ATmega32__)
        #if F_CPU == 16000000
            case 2400:      REG_UBRRL=160;  REG_UBRRH=1; break;
            case 4800:      REG_UBRRL=207;  REG_UBRRH=0; break;
            case 9600:      REG_UBRRL=103;  REG_UBRRH=0; break;
            case 14400:     REG_UBRRL=68;   REG_UBRRH=0; break;
            case 19200:     REG_UBRRL=51;   REG_UBRRH=0; break;
            case 28800:     REG_UBRRL=34;   REG_UBRRH=0; break;
            case 57600:     REG_UBRRL=34;   REG_UBRRH=0; U2X_val=1; break;
            case 76800:     REG_UBRRL=12;   REG_UBRRH=0; break;
            case 115200:    REG_UBRRL=16;   REG_UBRRH=0; U2X_val=1; break;
            case 230400:    REG_UBRRL=8;    REG_UBRRH=0; U2X_val=1; break;
            case 250000:    REG_UBRRL=3;    REG_UBRRH=0; break;
            case 500000:    REG_UBRRL=1;    REG_UBRRH=0; break;
            case 1000000:   REG_UBRRL=0;    REG_UBRRH=0; break;
          #endif
      #endif
      #if defined (__AVR_ATmega324P__)
        #if F_CPU == 7372800
            case 2400:      REG_UBRRL=191;  REG_UBRRH=0; break;
            case 4800:      REG_UBRRL=95;  REG_UBRRH=0; break;
            case 9600:      REG_UBRRL=47;  REG_UBRRH=0; break;
            case 14400:     REG_UBRRL=31;   REG_UBRRH=0; break;
            case 19200:     REG_UBRRL=23;   REG_UBRRH=0; break;
            case 28800:     REG_UBRRL=15;   REG_UBRRH=0; break;
            case 38400:     REG_UBRRL=11;   REG_UBRRH=0; break;
            case 57600:     REG_UBRRL=7;   REG_UBRRH=0; break;
            case 76800:     REG_UBRRL=5;   REG_UBRRH=0; break;
            case 115200:    REG_UBRRL=3;   REG_UBRRH=0; break;
            case 230400:    REG_UBRRL=1;    REG_UBRRH=0; break;
            case 250000:    REG_UBRRL=1;    REG_UBRRH=0; break;
            case 500000:    REG_UBRRL=1;    REG_UBRRH=0; U2X_val=1; break;
            case 1000000:   REG_UBRRL=0;    REG_UBRRH=0; U2X_val=1; break;
          #endif
      #endif
        default: {
          ubrv=(F_CPU+(baudrate*8))/(baudrate*16)-1;
          REG_UBRRL = ubrv & 0xFF;
          REG_UBRRH = (ubrv >> 8) & 0xFF;
        } break;
    }



    // Interrupts kurz deaktivieren
    cli();

    // UART Receiver und Transmitter anschalten, Receive-Interrupt aktivieren
    REG_UCSRB = (1 << BIT_RXEN) | (1 << BIT_TXEN) | (1 << BIT_RXCIE);


    switch (parity) {
        case UART_NOPARITY: break;
        case UART_EVENPARITY: REG_UCSRC |= (1<<BIT_UPM1); break;
        case UART_ODDPARITY: REG_UCSRC |= (1<<BIT_UPM1)|(1<<BIT_UPM0); break;
    }
    switch (databits) {
        case 5: break;
        case 6: REG_UCSRC |= (1<<BIT_UCSZ0); break;
        case 7: REG_UCSRC |= (1<<BIT_UCSZ1); break;
        case 8: REG_UCSRC |= (1<<BIT_UCSZ0)|(1<<BIT_UCSZ1); break;
        case 9: REG_UCSRC |= (1<<BIT_UCSZ0)|(1<<BIT_UCSZ1)|(1<<BIT_UCSZ2); break;
    }
    switch (stopbits) {
        case 1: break;
        case 2: REG_UCSRC |= (1<<BIT_USBS); break;
    }

    if (U2X_val==1) REG_UCSRA = (1<<BIT_U2X); else REG_UCSRA=0;

    // Flush Receive-Buffer (entfernen evtl. vorhandener ungültiger Werte)
    do {
        // UDR auslesen (Wert wird nicht verwendet)
        REG_UDR;
    } while (REG_UCSRA & (1 << BIT_RXC));

    // Rücksetzen von Receive und Transmit Complete-Flags
    REG_UCSRA |= (1 << BIT_RXC) | (1 << BIT_TXC);

    // Global Interrupt-Flag wieder herstellen
    SREG = sreg;

    // FIFOs für Ein- und Ausgabe initialisieren
    fifo_init (&infifo,   inbuf, BUFSIZE_IN);
    fifo_init (&outfifo, outbuf, BUFSIZE_OUT);

}



// Empfangene Zeichen werden in die Eingabgs-FIFO gespeichert und warten dort
SIGNAL (SIG_UART_RECV) {
    _inline_fifo_put (&infifo, REG_UDR);
}

// Ein Zeichen aus der Ausgabe-FIFO lesen und ausgeben
// Ist das Zeichen fertig ausgegeben, wird ein neuer SIG_UART_DATA-IRQ getriggert
// Ist die FIFO leer, deaktiviert die ISR ihren eigenen IRQ.
SIGNAL (SIG_UART_DATA) {
    if (outfifo.count > 0)
        REG_UDR = _inline_fifo_get (&outfifo);
    else
        REG_UCSRB &= ~(1 << BIT_UDRIE);
}


/*int uart_putc (const uint8_t c)
{
    int ret=fifo_put (&outfifo, c);

    UCSRB |= (1 << UDRIE);

    return ret;
}*/

int uart_getc_nowait () {
    return fifo_get_nowait (&infifo);
}

uint8_t uart_getc_wait () {
    return fifo_get_wait (&infifo);
}

void uart_puts_P (PGM_P s) {
    while (1) {
        unsigned char c = pgm_read_byte (s);
        s++;
        if ('\0' == c)
            break;
        uart_putc (c);
    }
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

