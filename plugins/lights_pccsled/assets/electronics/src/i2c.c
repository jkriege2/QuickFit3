#include "i2c.h"
#include <util/delay.h>


/*
 * Do all the startup-time peripheral initializations: UART (for our
 * debug/test output), and TWI clock.
 */
void twi_init(void) {

  /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
#if defined(TWPS0)
  /* has prescaler (mega128 & newer) */
  TWSR = 0;
#endif

#if F_CPU < 3600000UL
  TWBR = 10;			/* smallest TWBR value, see note [5] */
#else
  TWBR = (F_CPU / TWI_CLOCK_KHZ - 16) / 2;
#endif
}

signed char twi_repeated_start(uint8_t address) {
    return twi_start(address);
}

signed char twi_start(uint8_t address) {
//
//    uint8_t redo=0;
//    uint8_t twst;
//
//    do {
//        TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
//        while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
//        twst = TW_STATUS & 0xF8;
////        switch (twst) {
////            case TW_REP_START:		/* OK, but should not happen */
////            case TW_START:
////               redo= TWI_MAX_TRIES; // DONE!!!
////              break;
////
////            case TW_MT_ARB_LOST:	/* Note [9] */
////              break; // REDO
////
////            default:
////              return 0;	/* error: not in start condition */
////                        /* NB: do /not/ send stop condition */
////        }
//        if ( (twst != TW_START) && (twst != TW_REP_START)) return 0;
//        if (twst==TW_START) break;
//        _delay_ms(20);
//        redo++;
//    } while (redo<TWI_MAX_TRIES);
//
//    // send device address
//    TWDR = address;
//    TWCR = (1<<TWINT) | (1<<TWEN);
//
//    // wail until transmission completed and ACK/NACK has been received
//    while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
//
//    // check value of TWI Status Register.
//    twst = TW_STATUS&0xF8;
//    if((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 0;
//
//    return 1;
    uint8_t   twst;

	// send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait until transmission completed
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_START) && (twst != TW_REP_START)) return 0;

	// send device address
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wail until transmission completed and ACK/NACK has been received
	while(!(TWCR & (1<<TWINT)));

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 0;

	return 1;
}


void twi_stop(void) {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
    // wait until stop condition is executed and bus released
    while(TWCR & (1<<TWSTO));
}

signed char twi_write( uint8_t data ) {
   uint8_t   twst;

    // send data to the previously addressed device
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);

    // wait until transmission completed
    while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */

    // check value of TWI Status Register
    twst = TW_STATUS&0xF8;
    if( twst != TW_MT_DATA_ACK) return 0;
    return 1;

}



uint8_t twi_read_ack(void) {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */

    return TWDR;
}


uint8_t twi_read_noack(void) {
    TWCR = (1<<TWINT) | (1<<TWEN);
    while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */

    return TWDR;
}


uint8_t twi_24c02_readbyte(uint8_t address, uint8_t* okk) {
    uint8_t data=0;
    uint8_t ok;
    ok=twi_start(TWI_SLA_24CXX | TWI_MODE_WRITE);
    if (ok!=0) ok=twi_write(address);
    else ok= 255;
    if (ok!=0) ok=twi_repeated_start(TWI_SLA_24CXX | TWI_MODE_READ);
    else ok= 254;
    if (ok!=0) data=twi_read_noack();
    else ok= 253;
    twi_stop();
    if (okk) *okk=ok;
    return data;
}

signed char twi_24c02_writebyte(uint8_t address, uint8_t data) {
    signed char ok;
    ok=twi_start(TWI_SLA_24CXX | TWI_MODE_WRITE);
    if (ok!=0) ok=twi_write(address);
    if (ok!=0) ok=twi_write(data);
    twi_stop();
    _delay_ms(6);
    return ok;
}

signed char twi_24c02_readbytes(uint8_t* target, uint8_t address, uint8_t len) {
    signed char ok=1;
    uint8_t i;
    for (i=0; i<len; i++) {
        target[i]=twi_24c02_readbyte(address+i,NULL);
    }
    return ok;
}


signed char twi_24c02_writebytes(uint8_t address, const uint8_t* data, uint8_t len) {
    signed char ok=1;
    uint8_t i;
    for (i=0; i<len; i++) {
        ok=twi_24c02_writebyte(address+i, data[i]);
        if (ok==0) break;
    }
    return ok;
}
