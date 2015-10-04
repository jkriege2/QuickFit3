/*
  this file:     Tool functions
  project:       Servo Controller
  author:        (c) 2011 by Jan Krieger
                 jan@jkrieger.de, www.jkrieger.de

  target system:     Atmel ATmega328, no external RAM
                     XTAL: 16 MHz !!!
  compiler (tested): avr-gcc 4.3.3
*/

#ifndef tools_h
#define tools_h

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








#define servo4_pwm_off() cbi(PORTD, PD2)
#define servo4_pwm_on() sbi(PORTD, PD2)

#define servo3_pwm_off() cbi(PORTD, PD5)
#define servo3_pwm_on() sbi(PORTD, PD5)

#define servo2_pwm_off() cbi(PORTB, PB0)
#define servo2_pwm_on() sbi(PORTB, PB0)

#define servo1_pwm_off() cbi(PORTC, PC3)
#define servo1_pwm_on() sbi(PORTC, PC3)

#define servo4_led_off() cbi(PORTD, PD3)
#define servo4_led_on() sbi(PORTD, PD3)
#define servo3_led_off() cbi(PORTD, PD6)
#define servo3_led_on() sbi(PORTD, PD6)
#define servo2_led_off() cbi(PORTB, PB1)
#define servo2_led_on() sbi(PORTB, PB1)
#define servo1_led_off() {}
#define servo1_led_on() {}

#define servo4_button() (!bit_is_set(PIND, PD4))
#define servo3_button() (!bit_is_set(PIND, PD7))
#define servo2_button() (!bit_is_set(PINB, PB2))
#define servo1_button() (!bit_is_set(PINC, PC2))

//Timer/Counter 1 A in CTC mode enable/disable, may interfere with
void timer1A_enable(uint16_t interrupts_per_sec);
#define timer1A_disable() { cbi(TIMSK1, OCIE1A); }

//Timer/Counter 2 A in CTC mode enable/disable
void timer2A_enable(uint16_t interrupts_per_sec);
#define timer2A_disable() { cbi(TIMSK2, OCIE2A); }



// read internal ADC (single ended input, external Vref)
// measurements gives the number of measurements to do and average over
uint16_t ADC_read(uint8_t channel, uint8_t measurements, uint16_t wait_us);


// write <data> to SPI
void spi_write(unsigned char data);
// write <data> to SPI and return whatever has been received
uint8_t spi_readwrite(unsigned char data);

void to_decimal(char* data, uint8_t decimals);



#endif /* setup_h */

