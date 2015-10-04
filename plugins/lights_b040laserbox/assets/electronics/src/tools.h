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


/*
  this file:     Tool functions

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
#define sbi(port, pin) port|=_BV(pin)
#define cbi(port, pin) port&= ~(_BV(pin))
#define Set_Bit(port, pin) sbi(port, pin)
#define Clear_Bit(port, pin) cbi(port, pin)




/*
  InitHardware() initialises the controller (IO-Lines, Timer, SPI etc.)
*/
void InitHardware(void);




//get status of control buttons ("menu", "on/off")
#define button_MENU() bit_is_set(PINA, PINA1)
#define button_ONOFF() bit_is_set(PINC, PINC5)

//get status of control buttons ("menu", "on/off")
#define button_IMENU() (!button_MENU())
#define button_IONOFF() (!button_ONOFF())

// get Status of input line PB2
#define line_ONOFFFINPUT() bit_is_set(PINB, PINB2)


//switch LCD backlight on/off
#define lcdback_on() sbi(PORTD, PD7);
#define lcdback_off() cbi(PORTD, PD7);
#define lcdback_set(v) {if(v>=0){lcdback_on();}else{lcdback_off();}}

//LOCK LED
#define ledlock_on() sbi(PORTD, PD4);
#define ledlock_off() cbi(PORTD, PD4);
#define ledlock_set(v) {if(v>=0){ledlock_on();}else{ledlock_off();}}

//ERROR LED
#define lederror_on() sbi(PORTD, PD5);
#define lederror_off() cbi(PORTD, PD5);
#define lederror_set(v) {if(v>=0){lederror_on();}else{lederror_off();}}

//POW LED
#define ledpow_on() sbi(PORTD, PD6);
#define ledpow_off() cbi(PORTD, PD6);
#define ledpow_set(v) {if(v>=0){ledpow_on();}else{ledpow_off();}}


//SWITCH LED (Laser on)
#define ledlaser_on() sbi(PORTA, PA4);
#define ledlaser_off() cbi(PORTA, PA4);
#define ledlaser_set(v) {if(v>=0){ledlaser_on();}else{ledlaser_off();}}




//Timer/Counter 1 A enable/disable
void timer1A_enable(uint16_t interrupts_per_sec);
#define timer1A_disable() { cbi(TIMSK1, OCIE1A); }

//Timer/Counter 1 B enable/disable
void timer1B_enable(uint16_t interrupts_per_sec);
#define timer1B_disable() { cbi(TIMSK1, OCIE1B); }

// Timer/Counter 2 as PWM
void PWM2A_enable(void) ;
void PWM2A_disable(void) ;
void PWM2A_set(uint8_t value) ;

// write <data> to SPI
void spi_write(unsigned char data);
// write <data> to SPI and return whatever has been received
uint8_t spi_readwrite(unsigned char data);

uint16_t ADC_read(uint8_t channel, uint8_t measurements, uint16_t wait_us);

#endif /* setup_h */

