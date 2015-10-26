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


// special characters for LCD display

#include <avr/pgmspace.h>

#ifndef _LCD_MYFONT_H
#define _LCD_MYFONT_H


#ifndef LCDC_IMPLEMENT
//extern uint8_t LCDC_INV_PLUS[8] PROGMEM;

//extern uint8_t LCDC_INV_MINUS[8] PROGMEM;
extern uint8_t LCDC_INV_ARROWUP[8] PROGMEM;
extern uint8_t LCDC_INV_ARROWDOWN[8] PROGMEM;
extern uint8_t LCDC_INV_ENTER[8] PROGMEM;
//extern uint8_t LCDC_ONOFF[8] PROGMEM;
extern uint8_t LCDC_INV_ONOFF[8] PROGMEM;
/*extern uint8_t LCDC_ALL1[8] PROGMEM;
extern uint8_t LCDC_BOX[8] PROGMEM;
extern uint8_t LCDC_FAN1[8] PROGMEM;
extern uint8_t LCDC_FAN2[8] PROGMEM;
extern uint8_t LCDC_FAN3[8] PROGMEM;
extern uint8_t LCDC_FAN4[8] PROGMEM;*/
#endif

#endif //_LCD_MYFONT_H
