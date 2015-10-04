/*
  this file:     configuration data for LED controller (global variables)
                 thisf also includes saving/reading configuration
				 to/from EEPROM
  project:       LED controller
  author:        (c) 2010 by Jan Krieger
                 jan@jkrieger.de, www.jkrieger.de

  target system: Atmel ATmega32, no external RAM
                 XTAL: 16 MHz !!!
  compiler:      avr-gcc 4.2.2
*/


#ifndef config_h
#define config_h

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <stdio.h>
#include "tools.h"


// memory locations of config-variables in EEPROM:
#define ADR_FIRSTTIME       0x00
#define ADR_FAN_START       0x02
#define ADR_FAN_FACTOR      0x04
#define ADR_FAN_OFFSET      0x06
#define ADR_LCD_BACKLIGHT   0x08
#define ADR_CURRENT         0x0A
#define ADR_BAUDRATE        0x0C
#define ADR_CURRENT_MAX     0x10
#define ADR_CURRENT_100     0x12
#define ADR_LED             0x14
#define ADR_IADC_MEASUREMENTS 0x16
#define ADR_IADC_WAIT       0x18
#define ADR_EADC_MEASUREMENTS 0x1A
#define ADR_EADC_WAIT       0x1C
#define ADR_LEDBLINK        0x1E


#ifdef GLOBALDEFINITIONS
// indicates whether system is in EDITOR mode
volatile int16_t FAN_START;
volatile uint16_t FAN_FACTOR;
volatile uint16_t FAN_OFFSET;
volatile uint8_t LCD_BACKLIGHT;
volatile uint16_t CURRENT;
volatile uint16_t CURRENT_MAX;
volatile uint16_t CURRENT_100;
volatile uint8_t LED;
volatile uint32_t BAUDRATE;
volatile uint16_t IADC_MEASUREMENTS;
volatile uint16_t IADC_WAIT;
volatile uint16_t EADC_MEASUREMENTS;
volatile uint16_t EADC_WAIT;
volatile uint16_t LEDBLINK;
volatile uint16_t LEDBLINK_compare;
#endif

#ifndef GLOBALDEFINITIONS
extern volatile int16_t FAN_START;
extern volatile uint16_t FAN_FACTOR;
extern volatile uint16_t FAN_OFFSET;
extern volatile uint8_t LCD_BACKLIGHT;
extern volatile uint16_t CURRENT;
extern volatile uint16_t CURRENT_MAX;
extern volatile uint16_t CURRENT_100;
extern volatile uint8_t LED;
extern volatile uint32_t BAUDRATE;
extern volatile uint16_t IADC_MEASUREMENTS;
extern volatile uint16_t IADC_WAIT;
extern volatile uint16_t EADC_MEASUREMENTS;
extern volatile uint16_t EADC_WAIT;
extern volatile uint16_t LEDBLINK;
extern volatile uint16_t LEDBLINK_compare;
#endif



// load and save config-values from/to EEPROM
void configSave(void);
void configLoad(void);
// save those parameters that are NOT set in the menu, but during "runtime", i.e. the current
void configSaveNonMenu(void);

// restore default settings
void configLoadStd(void);

// calculate missing parameters from configuration
void configCalc(void);

#endif /* config_h */


