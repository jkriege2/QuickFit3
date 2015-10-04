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


#define GLOBALDEFINITIONS
#include "config.h"

#define SAVETOEEPROM(param, adr, size) \
  { \
     uint8_t temp[size]; \
     uint8_t* d=(uint8_t*)&(param); \
     char write=0; \
     int i; \
     eeprom_read_block((void*)temp, (void*)(adr), size); \
     for (i=0; i<size; i++) { \
         if (temp[i]!=d[i]) { write =1; break; } \
     } \
     if (write==1) { \
         eeprom_write_block((void*)&(param), (void*)(adr), size); \
     } \
  } \

void configSave(void) {
    configCalc();
    eeprom_write_word((void*)ADR_FIRSTTIME, 0xEEEE);
    /*eeprom_write_block((void*)&LCD_BACKLIGHT, (void*)ADR_LCD_BACKLIGHT, 1);
    eeprom_write_block((void*)&LCD_CONTRAST, (void*)ADR_LCD_CONTRAST, 1);
    eeprom_write_block((void*)&BAUDRATE, (void*)ADR_BAUDRATE, 4);
    eeprom_write_block((void*)&MAX_POWER, (void*)ADR_MAX_POWER, 2);
    eeprom_write_block((void*)&DEBUG_MODE, (void*)ADR_DEBUG_MODE, 1);
    eeprom_write_block((void*)&EXTERNAL_INTENSITY_MODE, (void*)ADR_EXTERNAL_INTENSITY_MODE, 1);
    eeprom_write_block((void*)&EXTERNAL_ONOFF_MODE, (void*)ADR_EXTERNAL_ONOFF_MODE, 1);
    eeprom_write_block((void*)&EXTINTENSITY_MIN, (void*)ADR_EXTINTENSITY_MIN, 2);
    eeprom_write_block((void*)&EXTINTENSITY_MAX, (void*)ADR_EXTINTENSITY_MAX, 2);*/
    SAVETOEEPROM(LCD_BACKLIGHT, ADR_LCD_BACKLIGHT, 1);
    SAVETOEEPROM(LCD_CONTRAST, ADR_LCD_CONTRAST, 1);
    SAVETOEEPROM(BAUDRATE, ADR_BAUDRATE, 4);
    SAVETOEEPROM(MAX_POWER, ADR_MAX_POWER, 2);
    SAVETOEEPROM(DEBUG_MODE, ADR_DEBUG_MODE, 1);
    SAVETOEEPROM(EXTERNAL_INTENSITY_MODE, ADR_EXTERNAL_INTENSITY_MODE, 1);
    SAVETOEEPROM(EXTERNAL_ONOFF_MODE, ADR_EXTERNAL_ONOFF_MODE, 1);
    SAVETOEEPROM(EXTINTENSITY_MIN, ADR_EXTINTENSITY_MIN, 2);
    SAVETOEEPROM(EXTINTENSITY_MAX, ADR_EXTINTENSITY_MAX, 2);
    SAVETOEEPROM(WAVELENGTH, ADR_WAVELENGTH, sizeof(WAVELENGTH));
    SAVETOEEPROM(CHECK_LASERCONNECTED, ADR_CHECK_LASERCONNECTED, 1);
#ifdef TWO_LASERS
    SAVETOEEPROM(MAX_POWER2, ADR_MAX_POWER2, 2);
    SAVETOEEPROM(WAVELENGTH2, ADR_WAVELENGTH2, sizeof(WAVELENGTH2));
#endif
}

void configLoad(void) {
    if (eeprom_read_word((void*)ADR_FIRSTTIME)!=0xEEEE) {
        configLoadStd();
    } else {
        eeprom_read_block((void*)&LCD_BACKLIGHT, (void*)ADR_LCD_BACKLIGHT, 1);
        eeprom_read_block((void*)&LCD_CONTRAST, (void*)ADR_LCD_CONTRAST, 1);
        eeprom_read_block((void*)&BAUDRATE, (void*)ADR_BAUDRATE, 4);
        eeprom_read_block((void*)&MAX_POWER, (void*)ADR_MAX_POWER, 2);
        eeprom_read_block((void*)&POWER, (void*)ADR_POWER, 2);
        eeprom_read_block((void*)&CURRENT, (void*)ADR_CURRENT, 2);
        eeprom_read_block((void*)&DEBUG_MODE, (void*)ADR_DEBUG_MODE, 1);
        eeprom_read_block((void*)&EXTERNAL_INTENSITY_MODE, (void*)ADR_EXTERNAL_INTENSITY_MODE, 1);
        eeprom_read_block((void*)&EXTERNAL_ONOFF_MODE, (void*)ADR_EXTERNAL_ONOFF_MODE, 1);
        eeprom_read_block((void*)&EXTINTENSITY_MIN, (void*)ADR_EXTINTENSITY_MIN, 2);
        eeprom_read_block((void*)&EXTINTENSITY_MAX, (void*)ADR_EXTINTENSITY_MAX, 2);
        eeprom_read_block((void*)&WAVELENGTH, (void*)ADR_WAVELENGTH, sizeof(WAVELENGTH));
        eeprom_read_block((void*)&CHECK_LASERCONNECTED, (void*)ADR_CHECK_LASERCONNECTED, 1);
#ifdef TWO_LASERS
        eeprom_read_block((void*)&MAX_POWER2, (void*)ADR_MAX_POWER2, 2);
        eeprom_read_block((void*)&POWER2, (void*)ADR_POWER2, 2);
        eeprom_read_block((void*)&CURRENT2, (void*)ADR_CURRENT2, 2);
        eeprom_read_block((void*)&WAVELENGTH2, (void*)ADR_WAVELENGTH2, sizeof(WAVELENGTH2));
#endif
   }
    configCalc();
}


void configLoadStd(void) {
    LCD_BACKLIGHT=125;
    LCD_CONTRAST=16;
    BAUDRATE=115200;
    MAX_POWER=25;
    POWER=10;
	DEBUG_MODE=0;
    CURRENT=1000;
    WAVELENGTH=491.0;
    CHECK_LASERCONNECTED=1;

#ifdef TWO_LASERS
    MAX_POWER2=25;
    POWER2=10;
    CURRENT2=1000;
    WAVELENGTH2=491.0;
#endif

    EXTERNAL_INTENSITY_MODE=0;
    EXTERNAL_ONOFF_MODE=0;
    EXTINTENSITY_MIN=100;
    EXTINTENSITY_MAX=2500;

    configCalc();
    configSave();
}

void configCalc(void) {
    if (EXTINTENSITY_MAX<EXTINTENSITY_MIN) {
        uint16_t h=EXTINTENSITY_MAX;
        EXTINTENSITY_MAX=EXTINTENSITY_MIN;
        EXTINTENSITY_MIN=h;
    }
    if (EXTINTENSITY_MAX==EXTINTENSITY_MIN) {
        EXTINTENSITY_MAX=EXTINTENSITY_MIN+10;
    }
    EXTINTENSITY_FACTOR=10*MAX_POWER;
    EXTINTENSITY_OFFSET=EXTINTENSITY_MIN;
    EXTINTENSITY_RANGE=EXTINTENSITY_MAX-EXTINTENSITY_MIN;
}

void configSaveNonMenu(void) {
    //eeprom_write_block((void*)&POWER, (void*)ADR_POWER, 2);
    //eeprom_write_block((void*)&CURRENT, (void*)ADR_CURRENT, 2);
    SAVETOEEPROM(POWER, ADR_POWER, 2);
    SAVETOEEPROM(CURRENT, ADR_CURRENT, 2);
#ifdef TWO_LASERS
    SAVETOEEPROM(POWER2, ADR_POWER2, 2);
    SAVETOEEPROM(CURRENT2, ADR_CURRENT2, 2);
#endif
}
