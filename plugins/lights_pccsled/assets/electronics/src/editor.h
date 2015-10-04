/*
  this file:     editor for system para,eters (using LCD and push buttons)
  project:       Triggerbox for BEC experiment
  author:        (c) 2006 by Jan Krieger
                 jan@jkrieger.de, www.jkrieger.de

  target system: Atmel ATmega32, no external RAM
                 XTAL: 16 MHz !!!
  compiler:      avr-gcc 3.4.6


  in edit mode Timer/Counter1 generates about 10 interrupts per second.
  In each interrupt the routineeditorButtonInt() is called, which counts how
  long which button is pushed. This data is written to the editor_count_UP/DOWN/ENTER
  variables.

    the timer runs at 16MHz/1024 = 15,625 kHz
    ==> count from 0 to 1563. On counter=1563 there will be a compare match,
	    this makes about 10 overflow interrupts per second (use OCR1A)

    --> see void InitHardware() in tools.h

  A button has to be pushed at least EDITOR_BUTTON_FIRSTHIT interrupts to be recognized
  as pushed (prevents jittering from the mechanical switch). After beeing pushed for
  EDITOR_BUTTON_MODSTART timer cycles the button releses on "push" every EDITOR_BUTTON_MOD
  cycles. This generates a fast editing mode.
    - button once: single inc/dec
	- push button longer: after some time an inc/dec is released with a fast period (fast editing)


*/

#ifndef editor_h
#define editor_h

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/eeprom.h>


#include "tools.h"
#include "lcd.h"
#include "lcd_myfont.h"
#include "config.h"
#include "leds.h"





// number of parameters
#define EDITOR_MAXMODE 15

// from this cycle count on a button is recognized as pushed
#define EDITOR_BUTTON_FIRSTHIT 1


// number of cycles to wait for a change in fast editing mode
#define EDITOR_BUTTON_MOD 3
// number of cycles to wait until fast/slow editing (i.e. changing value while button is pressed) begind
#define EDITOR_BUTTON_FASTEDITDELAY 10
// number of cycles to wait until fast/slow editing (i.e. changing value while button is pressed) begind
#define EDITOR_BUTTON_FASTFASTEDITDELAY 50
// number of cycles to wait until fast/slow editing (i.e. changing value while button is pressed) begind
#define EDITOR_BUTTON_FASTFASTFASTEDITDELAY 100

// this function should return the state of the UP button
#define EDITOR_BUTTON_UP_FUNC() button_IUP()
// this function should return the state of the DOWN button
#define EDITOR_BUTTON_DOWN_FUNC() button_IDOWN()
// this function should return the state of the ENTER button
#define EDITOR_BUTTON_ENTER_FUNC() button_IMENU()
// this function should return the state of the ACCEPT button
#define EDITOR_BUTTON_ACCEPT_FUNC() button_IONOFF()

// character used to display an up arrow as button description
#define EDITOR_UP_GLYPH '\x01'
// character used to display an down arrow as button description
#define EDITOR_DOWN_GLYPH '\x02'
// character used to display an enter arrow as button description
#define EDITOR_ENTER_GLYPH '\x03'
// character used to display a ACCEPT as button description
#define EDITOR_ACCEPT_GLYPH 'A'


// these special glyphs are loaded when the editor starts up
#define EDITOR_UP_GLYPH_ADR LCDC_INV_ARROWUP
#define EDITOR_DOWN_GLYPH_ADR LCDC_INV_ARROWDOWN
#define EDITOR_ENTER_GLYPH_ADR LCDC_INV_ENTER
#define EDITOR_ACCEPT_GLYPH_ADR LCDC_INV_ENTER

// this function enables the timer used to display the editor
#define EDITOR_ENABLE_TIMER() timer1B_enable()

// this function disables the timer used to display the editor
#define EDITOR_DISABLE_TIMER() timer1B_disable()




typedef void(*lcd_load_default_glyphs_T)(void);






#ifndef IMPLEMENT_EDITOR_VARIABLES
// indicates whether system is in EDITOR mode
extern volatile unsigned char editor_enabled;
// LCD file variable for editor output
extern FILE * editor_LCD;
// mode the editor is in (i.e. parameter to be set)
extern volatile unsigned char editor_mode;
// counter variables for buttons
extern volatile int editor_count_UP;
extern volatile int editor_count_DOWN;
extern volatile int editor_count_ENTER;
extern volatile int editor_count_ACCEPT;
// function that loads the default glyphs again
extern volatile lcd_load_default_glyphs_T lcd_load_default_glyphs_P;
#endif



// starts the editor
void editorStart(FILE *LCD);

// this has to be called in
void editorButtonInt(void);

// shows an editor for parameter <emode> on LCD
void editorShow(unsigned char emode);

// edit the BAUD rate
void editor_baudrate(uint32_t *value);

// edit select an LED
void editor_LED(uint8_t *value);

// edit an unsigned int value.
//   data is written to / read from <value> (POINTER!!!)
//   values may lie between min and max
//   UP/DOWN increments/decrements the value by <inc>
//   finally the value is written to adress <adr> im EEPROM
void editor_uint16(uint16_t *value, uint16_t min, uint16_t max, uint16_t inc, uint16_t fastinc_factor);

// edit an unsigned int value.
//   data is written to / read from <value> (POINTER!!!)
//   values may lie between min and max
//   UP/DOWN increments/decrements the value by <inc>
//   finally the value is written to adress <adr> im EEPROM
void editor_uint8(uint8_t *value, uint8_t min, uint8_t max, uint8_t inc, uint8_t fastinc_factor);

// edit an unsigned int value, jumps from max to min!
//   data is written to / read from <value> (POINTER!!!)
//   values may lie between min and max
//   UP/DOWN increments/decrements the value by <inc>
//   finally the value is written to adress <adr> im EEPROM
void editor_uint8_turn(uint8_t *value, uint8_t min, uint8_t max, uint8_t inc, uint8_t fastinc_factor);

// restore standard settings or config (calls configLoadStd() from config.h)
void editor_restore(void);

// quit editor
void editor_quit(void);

#endif /* editor_h */

