/*
  this file:     general Timer routines to implement a clock
  project:       triggerable GPIB controller
  author:        (c) 2008 by Jan Krieger
                 jan@jkrieger.de, www.jkrieger.de
  compiler:      avr-gcc (GCC) 4.2.2
  controller:    Atmel AVR device with 16 Bit Timer/Counter 1
                 tested on ATmega32


  the methods defined here use the 16-Bit-Timer/Counter1 to implement
  a clock. The timer receives the crystal frequency divided by 1024, so
  there is one click every 64usec @ 16MHz quarz frequency.

  We use Output Compare Interrupt 1A to increment a time variable every
  10ms, so we have to let the timer count to 144. Then it has to be reset
  to 0 (done automatically in CTC mode). The variable is declared volatile
  so read operations are not removed during optimization by the compiler.

  Time is counted in two volatile variables:
    * timer is a general purpose timer that can be used as a system clock.
	  The variable is an unsigned 32-bit integer, so time may count up to
	  49 days.
	* timeout_timer is a 16-bit unsigned integer variable, so it may only
      count up to 65.535 seconds. Use this to implement timeouts by setting
      it to 0 and then continuously comparing it to your timeout value.
      This method is implemented in the GPIB send methods!
      Note that you should not use this variable concurrently  by more than
	  one function at a time!!! As this could scramble the value and lead to
	  unreliable behaviour of your program.

  NOTE:
    If you change the quarz frequency you will also have to change the compare
	value. ... see below
  NOTE:
    Not all methods defined here affect timer and timeout_timer variables. See the
	function documentation for details!
  NOTE:
    The globale variables are declared extern for normal #include of timer.h.
	When the file is included from timer.c the preprocessor symbol TIMER_IMPLEMENT_VARIABLES
	is defined and the variables are actually implemented. So there exists only
	one instance that can be accessed by al parts of the program.

*/

#ifndef timer_h
#define timer_h

#include <avr/interrupt.h>
#include "tools.h"

#ifdef TIMER_IMPLEMENT_VARIABLES
volatile uint32_t timer;
volatile int16_t timeout_timer;
#else
extern volatile uint32_t timer;
extern volatile int16_t timeout_timer;
#endif


/* this is the value until which the timer counts.
   if it reaches TIMER_COMPARE the T/C will be reset to 0
   and time will be increased by 10;

   XTAL frequency [MHz] | TIMER_COMPARE
  ----------------------+---------------
                16.0000 | 156
                14.7456 | 144
                12.0000 | 117
                 8.0000 | 78
                 4.0000 | 39
 */

#define TIMER_COMPARE 156

// initialize and start the timer, does not affect timer or timeout_timer
static inline void timer_start(void) {
    TCCR1A=0;
    TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10);
    OCR1A=288;
    TCNT1=0;
    sbi(TIMSK, OCIE1A);
};

// stop the timer, does not affect timer or timeout_timer
static inline void timer_stop(void) {
    cbi(TIMSK, OCIE1A);
};

// reset the hardware timer, sets timer and timeout_timer to 0
static inline void timer_reset(void) {
    timer_stop();
    timer=0;
    timeout_timer=0;
    timer_start();
};

// This method resets the timeout timer variable timeout_timer to 0
#define timeout_reset() timeout_timer=0

// returns non-zero (true) when the given timeout occured, i.e. timeout_timer>timeout_time
#define timeout_check(timeout_time) (timeout_timer>(timeout_time))
#endif /* timer_h */


