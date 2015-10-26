#ifndef I2C_H_INCLUDED
#define I2C_H_INCLUDED

// I2C/TWI library, based on code from http://www.nongnu.org/avr-libc/user-manual/group__twi__demo.html
// http://homepage.hispeed.ch/peterfleury/group__pfleury__ic2master.html



#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/twi.h>		/* Note [1] */

#define DEBUG 1


/*
 * TWI address for 24Cxx EEPROM:
 *
 * 1 0 1 0 E2 E1 E0 R/~W	24C01/24C02
 * 1 0 1 0 E2 E1 A8 R/~W	24C04
 * 1 0 1 0 E2 A9 A8 R/~W	24C08
 * 1 0 1 0 A10 A9 A8 R/~W	24C16
 */
#define TWI_SLA_24CXX	0b10100000	/* E2 E1 E0 = 0 0 0 */

#define TWI_MODE_READ 1
#define TWI_MODE_WRITE 0

// TWI clock in kilohertz
#define TWI_CLOCK_KHZ 100000UL

// how often to retry at max.
#define TWI_MAX_TRIES 10

/*
 * Do all the startup-time peripheral initializations: UART (for our
 * debug/test output), and TWI clock.
 */
void twi_init(void);

/* init TWI transmission to a device at the given address
 *
 * sends the start condition and returns !=0 on success (ACK received) or 0 on failure
 */
signed char twi_start(uint8_t address);

/* send a repeated start condition */
signed char twi_repeated_start(uint8_t address);

/* send STOP condition */
void twi_stop(void);

/* \brief send one byte
 *
 * return !=0 on success and 0 on failure
 */
signed char twi_write( uint8_t data );

// read one byte from device
uint8_t twi_read_ack(void);
uint8_t twi_read_noack(void);

uint8_t twi_24c02_readbyte(uint8_t address, uint8_t* ok);
signed char twi_24c02_writebyte(uint8_t address, uint8_t data);
signed char twi_24c02_readbytes(uint8_t* target, uint8_t address, uint8_t len);
signed char twi_24c02_writebytes(uint8_t address, const uint8_t* data, uint8_t len);

#endif // I2C_H_INCLUDED
