#ifndef LEDS_H_INCLUDED
#define LEDS_H_INCLUDED
#include <avr/pgmspace.h>


/** a list of LEDs and their typical and maximum forward current */

#ifndef LED_IMPLEMENT
extern uint8_t led_count;
extern PGM_P led_names[] PROGMEM;
extern uint16_t led_current100[] PROGMEM;
extern uint16_t led_currentmax[] PROGMEM;
#endif

#endif // LEDS_H_INCLUDED
