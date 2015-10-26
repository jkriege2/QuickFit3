#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "fifo.h" // erklärt im Artikel "FIFO mit avr-gcc"

// FIFO-Objekte und Puffer für die Ein- und Ausgabe

#define BUFSIZE_IN  16
uint8_t inbuf[BUFSIZE_IN];
fifo_t infifo;

#define BUFSIZE_OUT 16
uint8_t outbuf[BUFSIZE_OUT];
fifo_t outfifo;


void uart_init(uint32_t baudrate, uint8_t databits, uint8_t stopbits, uint8_t parity) {
    // set baud rate
    uint8_t sreg = SREG;

    uint8_t U2X_val=0;
    uint16_t ubrv;


    switch(baudrate) {
      #if defined (__AVR_ATmega32__)
        #if F_CPU == 16000000
            case 2400:      UBRRL=160;  UBRRH=1; break;
            case 4800:      UBRRL=207;  UBRRH=0; break;
            case 9600:      UBRRL=103;  UBRRH=0; break;
            case 14400:     UBRRL=68;   UBRRH=0; break;
            case 19200:     UBRRL=51;   UBRRH=0; break;
            case 28800:     UBRRL=34;   UBRRH=0; break;
            case 57600:     UBRRL=34;   UBRRH=0; U2X_val=1; break;
            case 76800:     UBRRL=12;   UBRRH=0; break;
            case 115200:    UBRRL=16;   UBRRH=0; U2X_val=1; break;
            case 230400:    UBRRL=8;    UBRRH=0; U2X_val=1; break;
            case 250000:    UBRRL=3;    UBRRH=0; break;
            case 500000:    UBRRL=1;    UBRRH=0; break;
            case 1000000:   UBRRL=0;    UBRRH=0; break;
          #endif
      #endif
        default: {
          ubrv=(F_CPU+(baudrate*8))/(baudrate*16)-1;
          UBRRL = ubrv & 0xFF;
          UBRRH = (ubrv >> 8) & 0xFF;
        } break;
    }



    // Interrupts kurz deaktivieren
    cli();

    // UART Receiver und Transmitter anschalten, Receive-Interrupt aktivieren
    UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);


    switch (parity) {
        case UART_NOPARITY: break;
        case UART_EVENPARITY: UCSRC |= (1<<URSEL)|(1<<UPM1); break;
        case UART_ODDPARITY: UCSRC |= (1<<URSEL)|(1<<UPM1)|(1<<UPM0); break;
    }
    switch (databits) {
        case 5: break;
        case 6: UCSRC |= (1<<URSEL)|(1<<UCSZ0); break;
        case 7: UCSRC |= (1<<URSEL)|(1<<UCSZ1); break;
        case 8: UCSRC |= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1); break;
        case 9: UCSRC |= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1)|(1<<UCSZ2); break;
    }
    switch (stopbits) {
        case 1: break;
        case 2: UCSRC |= (1<<URSEL)|(1<<USBS); break;
    }

    if (U2X_val==1) UCSRA = (1<<U2X); else UCSRA=0;

    // Flush Receive-Buffer (entfernen evtl. vorhandener ungültiger Werte)
    do {
        // UDR auslesen (Wert wird nicht verwendet)
        UDR;
    } while (UCSRA & (1 << RXC));

    // Rücksetzen von Receive und Transmit Complete-Flags
    UCSRA |= (1 << RXC) | (1 << TXC);

    // Global Interrupt-Flag wieder herstellen
    SREG = sreg;

    // FIFOs für Ein- und Ausgabe initialisieren
    fifo_init (&infifo,   inbuf, BUFSIZE_IN);
    fifo_init (&outfifo, outbuf, BUFSIZE_OUT);

}



// Empfangene Zeichen werden in die Eingabgs-FIFO gespeichert und warten dort
SIGNAL (USART_RXC_vect  ) {
    _inline_fifo_put (&infifo, UDR);
}

// Ein Zeichen aus der Ausgabe-FIFO lesen und ausgeben
// Ist das Zeichen fertig ausgegeben, wird ein neuer SIG_UART_DATA-IRQ getriggert
// Ist die FIFO leer, deaktiviert die ISR ihren eigenen IRQ.
SIGNAL (USART_UDRE_vect ) {
    if (outfifo.count > 0)
        UDR = _inline_fifo_get (&outfifo);
    else
        UCSRB &= ~(1 << UDRIE);
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

