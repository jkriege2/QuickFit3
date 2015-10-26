// special characters for LCD display

#include <avr/pgmspace.h>

#ifndef _LCD_MYFONT_H
#define _LCD_MYFONT_H


#ifndef LCDC_IMPLEMENT
extern uint8_t LCDC_INV_PLUS[8] PROGMEM;

extern uint8_t LCDC_INV_MINUS[8] PROGMEM;
extern uint8_t LCDC_INV_ARROWUP[8] PROGMEM;
extern uint8_t LCDC_INV_ARROWDOWN[8] PROGMEM;
extern uint8_t LCDC_INV_ENTER[8] PROGMEM;
extern uint8_t LCDC_ONOFF[8] PROGMEM;
extern uint8_t LCDC_INV_ONOFF[8] PROGMEM;
extern uint8_t LCDC_ALL1[8] PROGMEM;
extern uint8_t LCDC_BOX[8] PROGMEM;
extern uint8_t LCDC_FAN1[8] PROGMEM;
extern uint8_t LCDC_FAN2[8] PROGMEM;
extern uint8_t LCDC_FAN3[8] PROGMEM;
extern uint8_t LCDC_FAN4[8] PROGMEM;
#endif

#endif //_LCD_MYFONT_H