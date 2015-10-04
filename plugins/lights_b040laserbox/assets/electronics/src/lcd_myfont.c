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


#define LCDC_IMPLEMENT
#include "lcd_myfont.h"
/*


// an inverted +

//    4 3 2 1 0
// 0  + + + + +
// 1  + +   + +
// 2  + +   + +
// 3
// 4  + +   + +
// 5  + +   + +
// 6  + + + + +
// 7  + + + + +
uint8_t LCDC_INV_PLUS[8] PROGMEM = {
    0b11111,
    0b11011,
    0b11011,
    0b00000,
    0b11011,
    0b11011,
    0b11111,
    0b11111
};


// an inverted -

//    4 3 2 1 0
// 0  + + + + +
// 1  + + + + +
// 2  + + + + +
// 3
// 4  + + + + +
// 5  + + + + +
// 6  + + + + +
// 7  + + + + +
uint8_t LCDC_INV_MINUS[8] PROGMEM = {
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

*/
// an inverted uparrow

//    4 3 2 1 0
// 0  + + + + +
// 1  + +   + +
// 2  +       +
// 3    +   +
// 4  + +   + +
// 5  + +   + +
// 6  + +   + +
// 7  + + + + +
uint8_t LCDC_INV_ARROWUP[8] PROGMEM = {
    0b11111,
    0b11011,
    0b10001,
    0b01010,
    0b11011,
    0b11011,
    0b11011,
    0b11111
};
// an inverted downarrow

//    4 3 2 1 0
// 0  + + + + +
// 1  + +   + +
// 2  + +   + +
// 3  + +   + +
// 4    +   +
// 5  +       +
// 6  + +   + +
// 7  + + + + +
uint8_t LCDC_INV_ARROWDOWN[8] PROGMEM = {
    0b11111,
    0b11011,
    0b11011,
    0b11011,
    0b01010,
    0b10001,
    0b11011,
    0b11111
};


// an inverted enter

//    4 3 2 1 0
// 0  + + + + +
// 1  + + +   +
// 2  + + +   +
// 3  + + +   +
// 4  +   +   +
// 5          +
// 6  +   + + +
// 7  + + + + +
uint8_t LCDC_INV_ENTER[8] PROGMEM = {
    0b11111,
    0b11101,
    0b11101,
    0b11101,
    0b10101,
    0b00001,
    0b10111,
    0b11111
};

/*
// an 0/1

//    4 3 2 1 0
// 0    +
// 1  +   +
// 2  +   +   +
// 3    +   +
// 4      +   +
// 5    +   + +
// 6  +       +
// 7          +
uint8_t LCDC_ONOFF[8] PROGMEM = {
    0b01000,
    0b10100,
    0b10101,
    0b01010,
    0b00101,
    0b01011,
    0b10001,
    0b00001
};
*/

// an inverted 0/1

//    4 3 2 1 0
// 0  +   + + +
// 1    +   +
// 2  +   +   +
// 3  + +   + +
// 4  +   + + +
// 5    + +   +
// 6  + + +   +
// 7  + + +   +
uint8_t LCDC_INV_ONOFF[8] PROGMEM = {
    0b10111,
    0b01010,
    0b10101,
    0b11011,
    0b10111,
    0b01101,
    0b11101,
    0b11101
};
/*
// an all 1

//    4 3 2 1 0
// 0  + + + + +
// 1  + + + + +
// 2  + + + + +
// 3  + + + + +
// 4  + + + + +
// 5  + + + + +
// 6  + + + + +
// 7  + + + + +
uint8_t LCDC_ALL1[8] PROGMEM = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

// an all 1

//    4 3 2 1 0
// 0  + + + + +
// 1  +       +
// 2  +       +
// 3  +       +
// 4  +       +
// 5  +       +
// 6  +       +
// 7  + + + + +
uint8_t LCDC_BOX[8] PROGMEM = {
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11111
};

// an fan 1

//    4 3 2 1 0
// 0        +
// 1        +
// 2      + +
// 3    + + +
// 4    + +
// 5    +
// 6    +
// 7
uint8_t LCDC_FAN1[8] PROGMEM = {
    0b00010,
    0b00010,
    0b00110,
    0b01110,
    0b01100,
    0b01000,
    0b01000,
    0b00000
};

// an fan 2

//    4 3 2 1 0
// 0  +
// 1    +
// 2      +
// 3    + + +
// 4      +
// 5        +
// 6          +
// 7
uint8_t LCDC_FAN2[8] PROGMEM = {
    0b10000,
    0b01000,
    0b00100,
    0b01110,
    0b00100,
    0b00010,
    0b00001,
    0b00000
};

// an fan 1

//    4 3 2 1 0
// 0
// 1
// 2  + + +
// 3    + + +
// 4      + + +
// 5
// 6
// 7
uint8_t LCDC_FAN3[8] PROGMEM = {
    0b00000,
    0b00000,
    0b11100,
    0b01110,
    0b00111,
    0b00000,
    0b00000,
    0b00000
};

// an fan 4

//    4 3 2 1 0
// 0          +
// 1        +
// 2      +
// 3    + + +
// 4      +
// 5    +
// 6  +
// 7
uint8_t LCDC_FAN4[8] PROGMEM = {
    0b00001,
    0b00010,
    0b00100,
    0b01110,
    0b00100,
    0b01000,
    0b10000,
    0b00000
};

*/
