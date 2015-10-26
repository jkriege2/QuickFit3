#define LED_IMPLEMENT

#include "leds.h"

uint8_t led_count=8;
//                            "             "
char led_string_0[] PROGMEM = "custom";
char led_string_1[] PROGMEM = "LXHL-N*98 1W";
char led_string_2[] PROGMEM = "LXHL-M*1* 1W";
char led_string_3[] PROGMEM = "LXHL-L*3C 3W";
char led_string_4[] PROGMEM = "LXHL-L*5C 5W";
char led_string_5[] PROGMEM = "Z-Pow. P4 3W";
char led_string_6[] PROGMEM = "Z-Pow. P9 0.5W";
char led_string_7[] PROGMEM = "CL-L190 5.6W";


PGM_P led_names[] PROGMEM = {
    led_string_0,
    led_string_1,
    led_string_2,
    led_string_3,
    led_string_4,
    led_string_5,
    led_string_6,
    led_string_7
};

uint16_t led_current100[] PROGMEM = {
    0,
    350,
    350,
    700,
    700,
    350,
    150,
    480
};

uint16_t led_currentmax[] PROGMEM = {
    0,
    500,
    500,
    1000,
    1000,
    1000,
    200,
    800
};
