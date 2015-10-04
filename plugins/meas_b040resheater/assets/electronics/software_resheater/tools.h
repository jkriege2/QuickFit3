/*
  this file:     Tool functions header file
  project:       TEC Controller
  author:        Jan Soldat
                 jan.soldat@gmail.com

  target system:     Atmel ATmega32, no external RAM
                     XTAL: 16 MHz
  compiler (tested): avr-gcc 4.3.3
*/

#ifndef tools_h
#define tools_h

#include "output_driver.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdio.h>
#include <util/delay.h>


// define sbi/cbi command to set/clear single bit of port
#define sbi(port, pin) {port|=_BV(pin);}
#define cbi(port, pin) {port&= ~(_BV(pin));}


//switch the internal fan on/off
#define intfan_on() sbi(PORTD,PORTD5);
#define intfan_off() cbi(PORTD,PORTD5);


//get status of control buttons
#define button_ONOFF() bit_is_set(PINB, PINB0)
#define button_DOWN() bit_is_set(PINB, PINB2)
#define button_UP() bit_is_set(PINB, PINB1)
#define button_MENU() bit_is_set(PINB, PINB3)
#define button_ACCEPT() button_ONOFF()

#define button_IONOFF() (!button_ONOFF())
#define button_IDOWN() (!button_DOWN())
#define button_IUP() (!button_UP())
#define button_IMENU() (!button_MENU())
#define button_IACCEPT() (!button_ACCEPT())


//switch LED 2 (upper one next to the LCD) on and off
#define led_2_on() cbi(PORTC,PORTC5);
#define led_2_off() sbi(PORTC,PORTC5);

//switch LED 1 (lower one next to the LCD) on and off
#define led_1_on() cbi(PORTD,PORTD2);
#define led_1_off() sbi(PORTD,PORTD2);

#define led_3_red() {sbi(PORTC,PORTC6); cbi(PORTC,PORTC7)};
#define led_3_yellow() {cbi(PORTC,PORTC6); sbi(PORTC,PORTC7)};
#define led_3_off() {cbi(PORTC,PORTC6); cbi(PORTC,PORTC7)};

//switch LCD backlight on/off
#define lcdback_on() sbi(PORTC, PORTC0);
#define lcdback_off() cbi(PORTC, PORTC0);

#define cs_none() { sbi(PORTC, PORTC4); }
// enable chip select of ADC (disable DAC!)
#define cs_adc() { cs_none(); cbi(PORTC, PORTC4); }



//Timer/Counter 1 A enable/disable
void timer1A_enable(uint16_t interrupts_per_sec);
#define timer1_disable() { cbi(TIMSK, OCIE1A); }


void timer0_enable(void);
#define timer0_disable() { cbi(TIMSK, OCIE0); }



//reads the MCP3208 ADC <quantity> times on ch. <channel>
uint16_t read_adc(uint8_t channel, uint8_t quantity);
uint16_t read_adc_single(uint8_t channel);

//sends a character on the SPI interface
uint8_t spi_write(unsigned char data);

//read out kty81/220 temperature sensor via internal ADC
//taking the average of <quantity> measurements
//resulting ADC value still needs to be converted using temperature lookup table
uint16_t read_inttemp(uint8_t quantity);

// write <data> to SPI
uint8_t spi_write(unsigned char data);

//set V(ctli) voltage according to desired current
void set_dac_4921(uint16_t current);

//set potentiometer value. potentiometers are used as voltage dividers between GND and Vref=1.5V
//value=0 gives 0 V input voltage for Max1968, value=255 gives 1.5 V
void set_dac_41100(uint8_t value, uint8_t number);

//Initializes the hardware
void avr_init(void);


// save the current configuration in EEPROM (only update changed configs!)
void save_configuration(void);

// read the configuration from the EEPROM
void load_configuration(void);

// Timer/Counter 2 as PWM
void PWM2_enable(uint8_t prescaler) ;
void PWM2_disable(void) ;
void PWM2_set(uint8_t value) ;



// converts an ADC value to a temperature in units of 1/10 degrees
int16_t PT1002T(uint16_t adc, int16_t PT100_OFFSET);
// converts an ADC value to a temperature in units of 1/100 degrees
int32_t PT1002T_100thdeg(uint16_t adc, int16_t PT100_OFFSET);

void to_decimal(char* data, uint8_t decimals);


#endif /* tools_h */

