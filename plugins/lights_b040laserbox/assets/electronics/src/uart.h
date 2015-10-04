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



int uart_putc (const uint8_t c);


//extern int uart_putc (const uint8_t);
extern uint8_t uart_getc_wait(void);
extern int     uart_getc_nowait(void);

void uart_flush(void);


int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);


// Einen 0-terminierten String übertragen.
void uart_puts (const char *s);
void uart_puts_P (PGM_P s);

#endif /* _UART_H_ */
