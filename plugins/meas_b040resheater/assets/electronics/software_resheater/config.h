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
#define ADR_BAUDRATE        0x02
#define ADR_SET_TEMP        0x06
#define ADR_T_INPUT         0x08
#define ADR_PT100_OFFSET0   0x0A
#define ADR_SHUTDOWN        0x0C
#define ADR_FAN_MIN         0x0E
#define ADR_FAN_INCREASE    0x10
#define ADR_PT100_OFFSET1   0x12
#define ADR_PID_I           0x14
#define ADR_PID_IMAX        0x16
#define ADR_PID_P           0x1A
#define ADR_PID_D           0x1C


#ifdef GLOBALDEFINITIONS
// indicates whether system is in EDITOR mode
volatile uint32_t BAUDRATE;
volatile int16_t SET_TEMP;
volatile int8_t T_INPUT;
volatile int16_t PT100_OFFSET0;
volatile int16_t PT100_OFFSET1;
volatile int16_t SHUTDOWN;
volatile int16_t FAN_MIN;
volatile int16_t FAN_INCREASE;
volatile int16_t PID_I;
volatile int32_t PID_IMAX;
volatile int16_t PID_P;
volatile int16_t PID_D;

#endif

#ifndef GLOBALDEFINITIONS
extern volatile uint32_t BAUDRATE;
extern volatile int16_t SET_TEMP;
extern volatile int8_t T_INPUT;
extern volatile int16_t PT100_OFFSET0;
extern volatile int16_t PT100_OFFSET1;
extern volatile int16_t SHUTDOWN;
extern volatile int16_t FAN_MIN;
extern volatile int16_t FAN_INCREASE;
extern volatile int16_t PID_I;
extern volatile int32_t PID_IMAX;
extern volatile int16_t PID_P;
extern volatile int16_t PID_D;
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


