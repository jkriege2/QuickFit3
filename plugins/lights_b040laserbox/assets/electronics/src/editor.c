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


#define IMPLEMENT_EDITOR_VARIABLES
#include "editor.h"

// indicates whether system is in EDITOR mode
volatile unsigned char editor_enabled=0;


// LCD file variable for editor output
FILE * editor_LCD;

// mode the editor is in (i.e. parameter to be set)
volatile unsigned char editor_mode=0;

// counter variables for buttons
volatile int editor_count_ENTER=0;
volatile int editor_count_ACCEPT=0;
volatile int editor_count_EXIT=0;
volatile int editor_count_WHEEL=0;
volatile lcd_load_default_glyphs_T lcd_load_default_glyphs_P=NULL;

// contains 1 if anything in the editor changed (=> update display)
volatile uint8_t editor_changed=0;

void editor_baudrate(uint32_t *value) {
    uint8_t br=0;

    switch(*value) {
        case 1200: br=0; break;
        case 2400: br=1; break;
        case 4800: br=2; break;
        case 9600: br=3; break;
        case 14400: br=4; break;
        case 19200: br=5; break;
        case 28800: br=6; break;
        case 57600: br=7; break;
        case 115200: br=8; break;
        case 128000: br=9; break;
        case 250000: br=10; break;
        case 500000: br=11; break;
        case 1000000: br=12; break;

        default: br=3; break;
    }

    editor_uint8(&br, 0, 12, 1);


    switch(br) {
        case 0: *value=1200; break;
        case 1: *value=2400; break;
        case 2: *value=4800; break;
        case 3: *value=9600; break;
        case 4: *value=14400; break;
        case 5: *value=19200; break;
        case 6: *value=28800; break;
        case 7: *value=57600; break;
        case 8: *value=115200; break;
        case 9: *value=128000; break;
        case 10: *value=250000; break;
        case 11: *value=500000; break;
        case 12: *value=1000000; break;
    }
}


void editor_uint8_turn(uint8_t *value, uint8_t min, uint8_t max, uint8_t inc) {
    int16_t r=editor_count_WHEEL;
    if (EDITOR_BUTTON_SPEEDUP_WHEEL_FUNC()) r=r*10;

    if (r>0) {
        *value+=r*inc;
        if (*value>max) *value=max;
        editor_changed=1;
    } else if (r<0) {
        if (*value<(-r*inc)+min) *value=min;
        else *value+=r*inc;
        editor_changed=1;
    }
}

void editor_uint8(uint8_t *value, uint8_t min, uint8_t max, uint8_t inc) {
    int16_t r=editor_count_WHEEL;
    if (EDITOR_BUTTON_SPEEDUP_WHEEL_FUNC()) r=r*10;

    if (r>0) {
        *value+=r*inc;
        if (*value>max) *value=max;
        editor_changed=1;
    } else if (r<0) {
        if (*value<(-r*inc)+min) *value=min;
        else *value+=r*inc;
        editor_changed=1;
    }
}


void editor_uint16(uint16_t *value, uint16_t min, uint16_t max, uint16_t inc) {
    int16_t r=editor_count_WHEEL;
    if (EDITOR_BUTTON_SPEEDUP_WHEEL_FUNC()) r=r*10;

    if (r>0) {
        *value+=r*inc;
        if (*value>max) *value=max;
        editor_changed=1;
    } else if (r<0) {
        if (*value<(-r*inc)+min) *value=min;
        else *value+=r*inc;
        editor_changed=1;
    }
}

void editor_int16(int16_t *value, int16_t min, int16_t max, int16_t inc) {
    int16_t r=editor_count_WHEEL;
    if (EDITOR_BUTTON_SPEEDUP_WHEEL_FUNC()) r=r*10;

    if (r>0) {
        *value+=r*inc;
        if (*value>max) *value=max;
        editor_changed=1;
    } else if (r<0) {
        if (*value<(-r*inc)+min) *value=min;
        else *value+=r*inc;
        editor_changed=1;
    }
}

void editor_double(double *value, double min, double max, double inc) {
    double r=editor_count_WHEEL;
    if (EDITOR_BUTTON_SPEEDUP_WHEEL_FUNC()) r=r*10.0;
    if (r>0) {
        *value+=r*inc;
        if (*value>max) *value=max;
        editor_changed=1;
    } else if (r<0) {
        if (*value<(-r*inc)+min) *value=min;
        else *value+=r*inc;
        editor_changed=1;
    }
}

void editor_restore(void) {
    if (editor_count_ACCEPT == EDITOR_BUTTON_FIRSTHIT) {
        configLoadStd();
        editor_changed=1;
        fprintf_P(editor_LCD, PSTR("\ndefaults restored           \r ... ... ... ...                 "));
        for (int i=0; i<4; i++) {
            PWM2A_set(0);
            _delay_ms(125);
            PWM2A_set(LCD_BACKLIGHT);
            _delay_ms(125);
        }
    }
}

void editor_quit(int which_count) {
    if (which_count == EDITOR_BUTTON_FIRSTHIT) {
        editor_enabled=0;
        // wait some time (1 sec)
        fprintf_P(editor_LCD, PSTR("\fsaving ...      \n                "));
        editor_changed=1;
        configSave();
        for (int i=0; i<4; i++) {
            PWM2A_set(0);
            _delay_ms(125);
            PWM2A_set(LCD_BACKLIGHT);
            _delay_ms(125);
        }
    }
}


void editorShow(unsigned char emode) {
    uint8_t show_updown=1;
    uint8_t show_accept=0;



    switch(emode) {
    case 1:
        editor_int16(&MAX_POWER, 0, 2000, 1);
        lcd_clear();
        fprintf(editor_LCD, "\fmax. laser power\n = %d mW", MAX_POWER);
        break;
    case 2: //{
        editor_double(&WAVELENGTH, 200, 2000, 0.1);
        lcd_clear();
        //char a[10];
        //dtostrf(WAVELENGTH, 6, 1, a);
        fprintf(editor_LCD, "\fwavelength\n = %6.1f nm", WAVELENGTH);
        //fprintf(editor_LCD, "\fwavelength\n = %s nm", a); }
        break;
#ifdef TWO_LASERS
    case 3:
        editor_int16(&MAX_POWER2, 0, 2000, 1);
        lcd_clear();
        fprintf(editor_LCD, "\fmax. laser pow 2\n = %d mW", MAX_POWER2);
        break;
    case 4: //{
        editor_double(&WAVELENGTH2, 200, 2000, 0.1);
        lcd_clear();
        //char a[10];
        //dtostrf(WAVELENGTH, 6, 1, a);
        fprintf(editor_LCD, "\fwavelength2\n = %6.1f nm", WAVELENGTH2);
        //fprintf(editor_LCD, "\fwavelength\n = %s nm", a); }
        break;
#endif
#ifdef ENABLEDEBUGMODE
    case EDITOR_MAXMODE-10:
        editor_uint8_turn(&DEBUG_MODE, 0, 1, 1);
        lcd_clear();
        fprintf(editor_LCD, "\fdebug mode      \n = %s      ", (DEBUG_MODE==0)?"off":"on");
        break;
#endif
    case EDITOR_MAXMODE-9:
        editor_uint8_turn(&EXTERNAL_INTENSITY_MODE, 0, 1, 1);
        lcd_clear();
        fprintf(editor_LCD, "\fext. intensity  \n enable = %s   ", (EXTERNAL_INTENSITY_MODE==0)?"off":"on");
        break;

    case EDITOR_MAXMODE-8:
        editor_uint16(&EXTINTENSITY_MIN, 0, 2560, 5);
        lcd_clear();
        fprintf(editor_LCD, "\fext. intensity  \n min = %d   ", EXTINTENSITY_MIN);
        break;
    case EDITOR_MAXMODE-7:
        editor_uint16(&EXTINTENSITY_MAX, 0, 2560, 5);
        lcd_clear();
        fprintf(editor_LCD, "\fext. intensity  \n max = %d   ", EXTINTENSITY_MAX);
        break;


    case EDITOR_MAXMODE-6:
        editor_uint8_turn(&EXTERNAL_ONOFF_MODE, 0, 1, 1);
        lcd_clear();
        fprintf(editor_LCD, "\fext. on/off mode\n = %s      ", (EXTERNAL_ONOFF_MODE==0)?"off":"on");
        break;

    case EDITOR_MAXMODE-5:
        editor_uint8_turn(&CHECK_LASERCONNECTED, 0, 1, 1);
        lcd_clear();
        fprintf(editor_LCD, "\fcheck laser     \nconnected = %s", (CHECK_LASERCONNECTED==0)?"no":"yes");
        break;


    case EDITOR_MAXMODE-4:
        editor_baudrate(&BAUDRATE);
        lcd_clear();
        fprintf(editor_LCD, "\fbaud rate\n = %ld", BAUDRATE);
        break;
    case EDITOR_MAXMODE-3:
        editor_uint8(&LCD_CONTRAST, 0, 64, 1);
        lcd_clear();
        fprintf(editor_LCD, "\fLCD contrast\n = %2d", LCD_CONTRAST);
        lcd_setcontrast(LCD_CONTRAST);
        break;
    case EDITOR_MAXMODE-2:
        editor_uint8(&LCD_BACKLIGHT, 0, 255, 5);
        lcd_clear();
        fprintf(editor_LCD, "\fLCD backlight\n = %3d", LCD_BACKLIGHT);
        PWM2A_set(LCD_BACKLIGHT);
        break;
    case EDITOR_MAXMODE-1:
        editor_restore();
        show_updown=0;
        show_accept=1;
        lcd_clear();
        fprintf_P(editor_LCD, PSTR("\frestore defaults.\npress A"));
        PWM2A_set(LCD_BACKLIGHT);
        break;
    case EDITOR_MAXMODE:
        editor_quit(editor_count_ACCEPT);
        show_updown=0;
        show_accept=1;
        lcd_clear();
        fprintf_P(editor_LCD, PSTR("\fexit menu         \npress A          "));
        break;
    }

    if (show_updown!=0) {
        lcd_pos_string(0, 3, " \x02\x01");
        lcd_pos_string(7, 3, "E");
    } else {
        if (show_accept!=0) {
            lcd_pos_string(0, 3, "  A ");
            lcd_pos_string(7, 3, " ");
        } else {
            lcd_pos_string(0, 3, "   ");
            lcd_pos_string(7, 3, "E");
        }
    }
    lcd_pos_string(11, 3, "\x03");

    editor_changed=0;
};




void editorButtonInt(void) {
    editor_changed=0;

    if (editor_count_ENTER>0xFFFA) editor_count_ENTER=1;
    if (editor_count_ACCEPT>0xFFFA) editor_count_ACCEPT=1;
    if (editor_count_EXIT>0xFFFA) editor_count_EXIT=1;

    if (EDITOR_BUTTON_ENTER_FUNC()) {
        editor_count_ENTER+=1;
    } else editor_count_ENTER=0;
    if (EDITOR_BUTTON_ACCEPT_FUNC()) {
        editor_count_ACCEPT+=1;
    } else editor_count_ACCEPT=0;
    if (EDITOR_BUTTON_EXIT_FUNC()) {
        editor_count_EXIT+=1;
    } else editor_count_EXIT=0;
    editor_count_WHEEL=EDITOR_ROTENCODER_READ();
    if (editor_count_WHEEL!=0) editor_changed=1;

    // for every change of editor mode you have to push ENTER
    // then you can change the next parameter
    if (editor_count_ENTER==1) {
        editor_mode+=1;
        editor_changed=1;
        editor_count_WHEEL=0;
    }

    if (editor_count_ACCEPT==1) {
        editor_changed=1;
    }

    if (editor_count_EXIT==1) {
        editor_quit(editor_count_EXIT);
        editor_enabled=0;
    }

    if (editor_mode>EDITOR_MAXMODE) editor_mode=1;

}



void editorStart(FILE *LCD) {
    editor_LCD=LCD;
    /*editor_count_UP=0;
    editor_count_DOWN=0;*/
    editor_count_ENTER=0;
    editor_enabled=1;
    editor_mode=1;
    lcd_define_letter_P(1, EDITOR_UP_GLYPH_ADR);
    lcd_define_letter_P(2, EDITOR_DOWN_GLYPH_ADR);
    lcd_define_letter_P(3, EDITOR_ENTER_GLYPH_ADR);
    lcd_define_letter_P(4, EDITOR_ACCEPT_GLYPH_ADR);
    lcd_home();
    lcd_clear();


    editor_changed=1;
    editorShow(editor_mode);
    EDITOR_BUTTON_ACCEPT_FUNC();
    //EDITOR_ENABLE_TIMER();
    //sei();
    while (editor_enabled==1) {
        if (editor_changed==1) editorShow(editor_mode);
    }

    //EDITOR_DISABLE_TIMER();
    editor_enabled=0;
    if (lcd_load_default_glyphs_P!=NULL) {
        lcd_load_default_glyphs_P();
        lcd_home();
        lcd_clear();
    }
}



