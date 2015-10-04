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
  this file:     configuration data for LED controller (global variables)
                 this ile also includes saving/reading configuration
				 to/from EEPROM

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
#define ADR_FIRSTTIME               0x00
#define ADR_LCD_BACKLIGHT           0x02
#define ADR_LCD_CONTRAST            0x04
#define ADR_BAUDRATE                0x06
#define ADR_MAX_POWER               0x0A
#define ADR_POWER                   0x0C
#define ADR_CURRENT                 0x0E
#define ADR_DEBUG_MODE              0x10
#define ADR_EXTERNAL_INTENSITY_MODE 0x12
#define ADR_EXTERNAL_ONOFF_MODE     0x14
#define ADR_EXTINTENSITY_MIN        0x16
#define ADR_EXTINTENSITY_MAX        0x18
#define ADR_WAVELENGTH              0x1C
#ifdef TWO_LASERS
#  define ADR_MAX_POWER2              0x24
#  define ADR_POWER2                  0x26
#  define ADR_CURRENT2                0x28
#  define ADR_WAVELENGTH2             0x2A
#endif
#define ADR_CHECK_LASERCONNECTED      0x20


#ifdef GLOBALDEFINITIONS
// indicates whether system is in EDITOR mode
volatile uint8_t LCD_BACKLIGHT;
volatile uint8_t LCD_CONTRAST;
volatile uint32_t BAUDRATE;
volatile uint16_t MAX_POWER;
volatile int16_t POWER;
volatile uint16_t CURRENT;
volatile uint8_t DEBUG_MODE;

volatile uint8_t EXTERNAL_INTENSITY_MODE;
volatile uint8_t EXTERNAL_ONOFF_MODE;
volatile uint16_t EXTINTENSITY_MIN;
volatile uint16_t EXTINTENSITY_MAX;
volatile int32_t EXTINTENSITY_OFFSET;
volatile int32_t EXTINTENSITY_FACTOR;
volatile int32_t EXTINTENSITY_RANGE;
volatile double WAVELENGTH;
volatile uint8_t CHECK_LASERCONNECTED;

#ifdef TWO_LASERS
volatile double WAVELENGTH2;
volatile uint16_t MAX_POWER2;
volatile int16_t POWER2;
volatile uint16_t CURRENT2;
#endif
#endif

#ifndef GLOBALDEFINITIONS
extern volatile uint8_t LCD_BACKLIGHT;
extern volatile uint8_t LCD_CONTRAST;
extern volatile uint32_t BAUDRATE;
extern volatile uint16_t MAX_POWER;
extern volatile int16_t POWER;
extern volatile uint16_t CURRENT;
extern volatile uint8_t DEBUG_MODE;
extern volatile uint8_t EXTERNAL_INTENSITY_MODE;
extern volatile uint8_t EXTERNAL_ONOFF_MODE;
extern volatile uint16_t EXTINTENSITY_MIN;
extern volatile uint16_t EXTINTENSITY_MAX;
extern volatile int32_t EXTINTENSITY_OFFSET;
extern volatile int32_t EXTINTENSITY_FACTOR;
extern volatile int32_t EXTINTENSITY_RANGE;
extern volatile double WAVELENGTH;
extern volatile uint8_t CHECK_LASERCONNECTED;

#ifdef TWO_LASERS
extern volatile double WAVELENGTH2;
extern volatile uint16_t MAX_POWER2;
extern volatile int16_t POWER2;
extern volatile uint16_t CURRENT2;
#endif
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


