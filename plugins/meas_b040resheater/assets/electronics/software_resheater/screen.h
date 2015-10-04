#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

void lcd_basic_screen(FILE* lcd);
void lcd_display_temp(FILE* lcd, int8_t source, int16_t temp0, int16_t temp1, int16_t int_temp, int16_t set_temp);
void lcd_display_PWM(FILE* lcd, int16_t out, int16_t outmin, int16_t out_max);

//prints just the measured values (current, temperature 0,1,int) when not in menu
//void lcd_update_measured(FILE* lcd, int16_t temp0, int16_t temp1, int16_t int_temp, int16_t act_current, int16_t SET_CURRENT, uint8_t CONTROL_TYPE, uint8_t heating_state, uint8_t sensor_status);

//prints just values set by user (TARGET_TEMPERATUREerature, SET_CURRENT) when not in menu
//void lcd_update_set(FILE* lcd, int16_t SET_CURRENT, int16_t TARGET_TEMPERATURE, uint8_t CONTROL_TYPE);


//void lcd_normal_screen(FILE* lcd, int16_t temp0, int16_t temp1, int16_t int_temp, int16_t TARGET_TEMPERATURE, int16_t act_current, int16_t SET_CURRENT, uint8_t CONTROL_TYPE, uint8_t heating_state, uint8_t TEMPERATURE_SOURCE);


#endif // SCREEN_H_INCLUDED
