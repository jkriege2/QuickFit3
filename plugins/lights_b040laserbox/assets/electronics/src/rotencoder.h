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


// tool functions to use a rotationary encoder
// based on code from <http://www.mikrocontroller.net/articles/Drehgeber>
#ifndef ROTENCODER_H
#define ROTENCODER_H

#include <avr/io.h>


/** \brief define the PINx register of the A pin of rotary encoder 0*/
#define ROTENCODER0_A_PINREG PINA
/** \brief define the DDRx register of the A pin of rotary encoder 0 */
#define ROTENCODER0_A_DDR DDRA
/** \brief define the PINx register of the A pin of rotary encoder 0 */
#define ROTENCODER0_A_PIN PA7

/** \brief define the PINx register of the B pi nof rotary encoder 0 */
#define ROTENCODER0_B_PINREG PINA
/** \brief define the DDRx register of the B pin of rotary encoder 0 */
#define ROTENCODER0_B_DDR DDRA
/** \brief define the PINx register of the B pin of rotary encoder 0 */
#define ROTENCODER0_B_PIN PA6

/** \brief define the PINx register of the switch pin of rotary encoder 0 */
#define ROTENCODER0_SWITCH_PINREG PINA
/** \brief define the DDRx register of the A pin of rotary encoder 0 */
#define ROTENCODER0_SWITCH_DDR DDRA
/** \brief define the PINx register of the A pin of rotary encoder 0 */
#define ROTENCODER0_SWITCH_PIN PA5
/** \brief indicates whether [1] or not [0] a switch input is available */
#define ROTENCODER0_SWITCH_AVAILABLE 1

#define ENCODERTYPE_1STEP 0
#define ENCODERTYPE_2STEP 1
#define ENCODERTYPE_4STEP 4

/*! \brief indicates the encoder type

      - set to ENCODERTYPE_1STEP for one step encoders
      - set to ENCODERTYPE_2STEP for two step encoders
      - set to ENCODERTYPE_4STEP for four step encoders
    .
 */
#define ROTENCODER0_ENCODERTYPE ENCODERTYPE_4STEP

/** \brief this function initialises the ports and global variables for the rotary encoder 0 */
void ROTENCODER0_init(void);

/** \brief read the input steps since last read operation for rotary encoder 0 */
int16_t ROTENCODER0_read(void);

/** \brief interrupt service routine for rotary encoder 0
 *
 * This function has to be called on a regular basis (e.g. every 1ms) to
 * evaluate the encoder state. You may e.g. call this in a global timer compare
 * interrupt.
 */
void ROTENCODER0_isr(void);

/** \brief oush button pressed? */
#define ROTENCODER0_button() bit_is_set(ROTENCODER0_SWITCH_PINREG, ROTENCODER0_SWITCH_PIN)

#endif // ROTENCODER_H
