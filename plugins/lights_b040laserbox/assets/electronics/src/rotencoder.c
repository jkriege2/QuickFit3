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


#include "rotencoder.h"

// some global variable:
volatile int16_t ROTENCODER0_int_delta;
static int8_t ROTENCODER0_int_last;

// some macro magic:
#define RE0_PHASE_A     (ROTENCODER0_A_PINREG & 1<<ROTENCODER0_A_PIN)
#define RE0_PHASE_B     (ROTENCODER0_B_PINREG & 1<<ROTENCODER0_B_PIN)
#define sbi(port, pin) port|=_BV(pin)
#define cbi(port, pin) port&= ~(_BV(pin))



void ROTENCODER0_init(void) {
  cbi(ROTENCODER0_A_DDR, ROTENCODER0_A_PIN);
  cbi(ROTENCODER0_B_DDR, ROTENCODER0_B_PIN);
#if (ROTENCODER0_SWITCH_AVAILABLE!=0)
  cbi(ROTENCODER0_SWITCH_DDR, ROTENCODER0_SWITCH_PIN);
#endif

  int8_t newv;

  newv = 0;
  if( RE0_PHASE_A )
    newv = 3;
  if( RE0_PHASE_B )
    newv ^= 1;                   // convert gray to binary
  ROTENCODER0_int_last = newv;   // power on state
  ROTENCODER0_int_delta = 0;
}

int16_t ROTENCODER0_read(void) {
#if (ROTENCODER0_ENCODERTYPE == ENCODERTYPE_1STEP)
    int16_t val;

    val = ROTENCODER0_int_delta;
    ROTENCODER0_int_delta = 0;
    return val;                   // counts since last call
#elif  (ROTENCODER0_ENCODERTYPE == ENCODERTYPE_2STEP)
    int16_t val;

    val = ROTENCODER0_int_delta;
    ROTENCODER0_int_delta = val & 1;
    return val >> 1;
#elif  (ROTENCODER0_ENCODERTYPE == ENCODERTYPE_4STEP)
    int16_t val;

    val = ROTENCODER0_int_delta;
    ROTENCODER0_int_delta = val & 3;
    return val >> 2;
#endif
}

void ROTENCODER0_isr(void) {
    int16_t newv, diff;

    newv = 0;
    if( RE0_PHASE_A ) newv = 3;
    if( RE0_PHASE_B ) newv ^= 1;                   // convert gray to binary
    diff = ROTENCODER0_int_last - newv;                // difference last - new
    if( diff & 1 ){               // bit 0 = value (1)
        ROTENCODER0_int_last = newv;                 // store new as next last
        ROTENCODER0_int_delta += (diff & 2) - 1;        // bit 1 = direction (+/-)
    }
}
