/*
  this file:     Tool functions
  project:       Triggerbox for BEC experiment
  author:        (c) 2010 by Jan Krieger
                 jan@jkrieger.de, www.jkrieger.de

  target system:     Atmel ATmega32, no external RAM
                     XTAL: 16 MHz !!!
  compiler (tested): avr-gcc 4.3.3
*/

#ifndef setup_h
#define setup_h

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdio.h>
#include <util/delay.h>




// define sbi/cbi command to set/clear single bit of port
#define sbi(port, pin) {port|=_BV(pin);}
#define cbi(port, pin) {port &= ~(_BV(pin));}
#define toggle(port, pin) {port ^= (_BV(pin));}




/*
  InitHardware() initialises the controller (IO-Lines, Timer, SPI etc.)
*/
void InitHardware(void);




//get status of control buttons ("up", "down", "menu", "on/off")
#define button_UP() bit_is_set(PIND, PIND2)
#define button_DOWN() bit_is_set(PIND, PIND3)
#define button_MENU() bit_is_set(PIND, PIND4)
#define button_ONOFF() bit_is_set(PIND, PIND6)

//get status of control buttons ("up", "down", "menu", "on/off")
#define button_IUP() (!bit_is_set(PIND, PIND2))
#define button_IDOWN() (!bit_is_set(PIND, PIND3))
#define button_IMENU() (!bit_is_set(PIND, PIND4))
#define button_IONOFF() (!bit_is_set(PIND, PIND6))


//switch LED (under ON/OFF switch) on and off (0: off; !=0: on)
#define led_off() cbi(PORTD, PD5);
#define led_on() sbi(PORTD, PD5);
#define led_set(v) {if(v>=0){led_on();}else{led_off();}}

//switch LCD backlight on/off
#define lcdback_on() cbi(PORTB, PB3);
#define lcdback_off() sbi(PORTB, PB3);
#define lcdback_set(v) {if(v>=0){lcdback_on();}else{lcdback_off();}}


// set/unset TTL output
#define ttl_on() cbi(PORTB, PB1);
#define ttl_off() sbi(PORTB, PB1);

// switch current output on/off
#define ledcurrent_on() sbi(PORTA, PA4);
#define ledcurrent_off() cbi(PORTA, PA4);
#define ledcurrent_toggle() toggle(PORTA, PA4);


//Timer/Counter 1 A enable/disable
void timer1_enable(void);
#define timer1_disable() { cbi(TIMSK, OCIE1A); }

//Timer/Counter 1 B enable/disable
void timer1B_enable(void);
#define timer1B_disable() { cbi(TIMSK, OCIE1B); }

// Timer/Counter 0 as PWM
// also enable TOIE0: Timer/Counter0 Overflow Interrupt Enable
void PWM0_enable(void);
void PWM0_disable(void);
void PWM0_set(uint8_t value);

#define timer0_enableTOIE() { sbi(TIMSK, TOIE0); }
#define timer0_disableTOIE() { cbi(TIMSK, TOIE0); }

// Timer/Counter 2 as PWM
void PWM2_enable(void) ;
void PWM2_disable(void) ;
void PWM2_set(uint8_t value) ;

// read internal ADC (single ended input, external Vref)
// measurements gives the number of measurements to do and average over
uint16_t ADC_read(uint8_t channel, uint8_t measurements, uint16_t wait_us);

// read external ADC
// measurements gives the number of measurements to do and average over
int16_t extADC_read(uint8_t measurements, uint16_t wait_us);

/* read KTY81/220 temperature sensor


     sensor is connected to pin PA6 (34) and GND. A 4k7 resistor is connected from
	 PA6 to +5V:

                    4k7              ~~\  KTY817220
                    _______           __\____
	        +5V o---|_____|-----*-----|_____|-----| GND
			                    |          \__
			                    |
								v Utemp -> PA6

	 the KTY and the resistor form a voltage divider with Utemp = 5V * RKTY/(4700R+RKTY)

	 The datasheet of KTY81/220 states these values for the temperature:
	    0°C  RTKY=1630R
	   25°C  RKTY=2000R

	 Therefore Utemp = 1,287V @ 0°C
	           Utemp = 1,493V @ 25°C
     Leaving away the linearisation this gives a temperature coefficient of 0,0082 V/°C

     The ADC is connected to Vref=4,096V and has 10 bit resolution, so each step is 0,004V.
	 So the temperature from Utemp is:

	   T = (ADC - 322) * 0,004/0,0082 [°C] =
	     = (ADC - 322) * 0,4878 [°C]
	     = (ADC - 322) / 2,05 [°C]

	 The offset is simply the voltage @ 0°c converted to ADC units!

	 So here we use (with only small errors which are OK for this application:

		 T = (ADC-322) / 2


*/
#define TEMP_read() (((int16_t)ADC_read(6, 8, 5)-322)/2)


// pull all chip select lines (DAC & ADC) high, i.e. deselect all
#define CS_none() { sbi(PORTB, PB4); sbi(PORTB, PB2); }
// enable chip select of DAC (disable ADC!)
#define CS_DAC() { CS_none(); cbi(PORTB, PB4); }
// enable chip select of DAC (disable ADC!)
#define CS_ADC() { CS_none(); cbi(PORTB, PB2); }

// write <data> to SPI
void spi_write(unsigned char data);
// write <data> to SPI and return whatever has been received
uint8_t spi_readwrite(unsigned char data);

// set the MCP4921 DAC on the board (12 bit, 4,096V reference)
void DAC_set(uint16_t value);

#endif /* setup_h */

