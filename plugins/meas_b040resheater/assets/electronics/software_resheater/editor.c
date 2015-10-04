#define IMPLEMENT_EDITOR_VARIABLES
#include "editor.h"

// indicates whether system is in EDITOR mode
volatile unsigned char editor_enabled=0;


// LCD file variable for editor output
FILE * editor_LCD;

// mode the editor is in (i.e. parameter to be set)
volatile unsigned char editor_mode=0;

// counter variables for buttons
volatile int editor_count_UP=0;
volatile int editor_count_DOWN=0;
volatile int editor_count_ENTER=0;
volatile int editor_count_ACCEPT=0;
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

    editor_uint8(&br, 0, 12, 1, 1);


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


void editor_uint8_turn(uint8_t *value, uint8_t min, uint8_t max, uint8_t incp, uint8_t fastinc_factor) {
    uint8_t inc=incp;
    //if ( (editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ((editor_count_UP == EDITOR_BUTTON_FIRSTHIT )|| ((editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_UP % EDITOR_BUTTON_MOD==0))) {
        if (*value <= max-inc) {
            *value+=inc;
        } else {
            *value=min+(inc-(max-*value)-1);
        }
        editor_changed=1;
    }
    if (editor_count_DOWN == EDITOR_BUTTON_FIRSTHIT || ((editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_DOWN % EDITOR_BUTTON_MOD==0))) {
        if (*value >= min+inc) {
            *value-=inc;
            editor_changed=1;
        } else {
            *value=max-(inc-(*value-min)-1);
            editor_changed=1;
        }
    }

}

void editor_uint8(uint8_t *value, uint8_t min, uint8_t max, uint8_t incp, uint8_t fastinc_factor) {
    uint8_t inc=incp;
    //if ( (editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ((editor_count_UP == EDITOR_BUTTON_FIRSTHIT )|| ((editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_UP % EDITOR_BUTTON_MOD==0))) {
        if (*value < max-inc) {
            *value+=inc;
        } else {
            *value=max;
        }
        editor_changed=1;
    }
    if (editor_count_DOWN == EDITOR_BUTTON_FIRSTHIT || ((editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_DOWN % EDITOR_BUTTON_MOD==0))) {
        if (*value > min+inc) {
            *value-=inc;
            editor_changed=1;
        } else {
            *value=min;
            editor_changed=1;
        }
    }

}


void editor_uint16(uint16_t *value, uint16_t min, uint16_t max, uint16_t incp, uint16_t fastinc_factor) {
    uint16_t inc=incp;
    //if ( (editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if (editor_count_UP == EDITOR_BUTTON_FIRSTHIT || ((editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_UP % EDITOR_BUTTON_MOD==0))) {
        *value+=inc;
        editor_changed=1;
        if (*value>max) *value=max;
    }
    if (editor_count_DOWN == EDITOR_BUTTON_FIRSTHIT || ((editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_DOWN % EDITOR_BUTTON_MOD==0))) {
        if (*value > min+inc) {
            *value-=inc;
            editor_changed=1;
        } else {
            *value=min;
            editor_changed=1;
        }
    }
}


void editor_int16(int16_t *value, int16_t min, int16_t max, int16_t incp, int16_t fastinc_factor) {
    int16_t inc=incp;
    //if ( (editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if (editor_count_UP == EDITOR_BUTTON_FIRSTHIT || ((editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_UP % EDITOR_BUTTON_MOD==0))) {
        *value+=inc;
        editor_changed=1;
        if (*value>max) *value=max;
    }
    if (editor_count_DOWN == EDITOR_BUTTON_FIRSTHIT || ((editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_DOWN % EDITOR_BUTTON_MOD==0))) {
        if (*value > min+inc) {
            *value-=inc;
            editor_changed=1;
        } else {
            *value=min;
            editor_changed=1;
        }
    }
}


void editor_int32(int32_t *value, int32_t min, int32_t max, int32_t incp, int32_t fastinc_factor) {
    int32_t inc=incp;
    //if ( (editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if (editor_count_UP == EDITOR_BUTTON_FIRSTHIT || ((editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_UP % EDITOR_BUTTON_MOD==0))) {
        *value+=inc;
        editor_changed=1;
        if (*value>max) *value=max;
    }
    if (editor_count_DOWN == EDITOR_BUTTON_FIRSTHIT || ((editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_DOWN % EDITOR_BUTTON_MOD==0))) {
        if (*value > min+inc) {
            *value-=inc;
            editor_changed=1;
        } else {
            *value=min;
            editor_changed=1;
        }
    }
}

void editor_restore(void) {
    if (editor_count_ACCEPT == EDITOR_BUTTON_FIRSTHIT) {
        configLoadStd();
        editor_changed=1;
        fprintf(editor_LCD, "\ndefaults restored           \r ... ... ... ...                 ");
        _delay_ms(500);
    }
}

void editor_quit(void) {
    if (editor_count_ACCEPT == EDITOR_BUTTON_FIRSTHIT) {
        editor_enabled=0;
        // wait some time (1 sec)
        fprintf(editor_LCD, "\fsaving ...      \n                ");
        editor_changed=1;
        configSave();
        _delay_ms(500);
    }
}


void editorShow(unsigned char emode) {
    //char s[3];
    uint8_t show_updown=1;
    uint8_t noexit=0;


    char inp[12];

    switch(emode) {
    case 1:
        editor_baudrate(&BAUDRATE);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fBAUDRATE            \n = %ld                 ", BAUDRATE);
        break;
    case 2:
        editor_uint8_turn(&T_INPUT, 0, 2, 1, 1);
        if (editor_changed==1) {
            lcd_clear();
            fprintf(editor_LCD, "\fINPUT               \n =                     ");
            if (T_INPUT==0) {
                lcd_pos_string(3,2,"T0");
            } else if (T_INPUT==1) {
                lcd_pos_string(3,2,"T1");
            } else if (T_INPUT==2) {
                lcd_pos_string(3,2,"(T0+T1)/2");
            }
        }

        break;
    case 3:
        editor_int16(&PT100_OFFSET0, -1000, 1000, 1, 10);
        sprintf(inp, "%4d", PT100_OFFSET0);
        to_decimal(inp, 1);
        if (editor_changed==1) {
            lcd_clear();
            fprintf(editor_LCD, "\fPT100 OFFSET for T0 \n = %s °C             ", inp);
        }
        sprintf(inp, "%4d", PT1002T(read_adc(1,16), PT100_OFFSET0));
        to_decimal(inp, 1);
        fprintf(editor_LCD, "\vT0=%s°C ", inp);
        break;
    case 4:
        editor_int16(&PT100_OFFSET1, -1000, 1000, 1, 10);
        sprintf(inp, "%4d", PT100_OFFSET1);
        to_decimal(inp, 1);
        if (editor_changed==1) {
            lcd_clear();
            fprintf(editor_LCD, "\fPT100 OFFSET for T1 \n = %s °C             ", inp);
        }
        sprintf(inp, "%4d", PT1002T(read_adc(0,16), PT100_OFFSET1));
        to_decimal(inp, 1);
        fprintf(editor_LCD, "\vT1=%s°C ", inp);
        break;
    case 5:
        editor_int16(&PID_P, 0, 1000, 1, 10);
        if (editor_changed==1) {
            lcd_clear();
            fprintf(editor_LCD, "\fPID P factor        \n = %d                ", PID_P);
        }
        break;
    case 6:
        editor_int16(&PID_I, 0, 1000, 1, 10);
        if (editor_changed==1) {
            lcd_clear();
            fprintf(editor_LCD, "\fPID I factor        \n = %d                ", PID_I);
        }
        break;
    case 7:
        editor_int16(&PID_D, 0, 1000, 1, 10);
        if (editor_changed==1) {
            lcd_clear();
            fprintf(editor_LCD, "\fPID D factor        \n = %d                ", PID_D);
        }
        break;
    case 8:
        editor_int32(&PID_IMAX, 0, 1000000, 1, 10);
        if (editor_changed==1) {
            lcd_clear();
            fprintf(editor_LCD, "\fPID I_MAX           \n = %ld                ", PID_IMAX);
        }
        break;


    case 9:
        editor_uint16(&FAN_MIN, 0, 100, 1, 2);
        if (editor_changed==1) {
            lcd_clear();
            fprintf(editor_LCD, "\fFAN START TEMP.     \n = %d °C             ", FAN_MIN);
        }
        break;
    case 10:
        editor_int16(&FAN_INCREASE, 0, 100, 1, 2);
        if (editor_changed==1) {
            lcd_clear();
            sprintf(inp, "%4d", FAN_INCREASE);
            to_decimal(inp, 1);
            fprintf(editor_LCD, "\fFAN INCREASE        \n = %s steps/°C             ", inp);
        }
        break;






    case EDITOR_MAXMODE-1:
        editor_restore();
        show_updown=0;
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\frestore defeault    \npress %c             ", EDITOR_ACCEPT_GLYPH);
        noexit=1;
        break;
    case EDITOR_MAXMODE:
        editor_quit();
        show_updown=0;
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fDONE !!!            \npress %c             ", EDITOR_ACCEPT_GLYPH);
        noexit=1;
        break;
    }

    if (noexit!=1) editor_quit();


    //if (emode<EDITOR_MAXMODE-1) {
        if (show_updown==0) {
            lcd_pos_char(7,4,' ');
            lcd_pos_char(12,4,' ');
        } else {
            lcd_pos_char(7,4,EDITOR_DOWN_GLYPH);
            lcd_pos_char(12,4,EDITOR_UP_GLYPH);
        }

        lcd_pos_char(2,4,EDITOR_ENTER_GLYPH);
        lcd_pos_char(17,4,EDITOR_ACCEPT_GLYPH);
    /*} else {
        lcd_pos_char(10,3,EDITOR_ENTER_GLYPH);
        lcd_pos_char(2,3,EDITOR_ACCEPT_GLYPH);
    }*/
    editor_changed=0;
};




void editorButtonInt(void) {
    EDITOR_DISABLE_TIMER();
    uint8_t sreg=SREG;
    cli();
    editor_changed=0;
    if (editor_count_UP>0xFFFA) editor_count_UP=1;      //reset counters before overflow
    if (editor_count_DOWN>0xFFFA) editor_count_DOWN=1;
    if (editor_count_ENTER>0xFFFA) editor_count_ENTER=1;
    if (editor_count_ACCEPT>0xFFFA) editor_count_ACCEPT=1;

    if (EDITOR_BUTTON_UP_FUNC()) editor_count_UP+=1;
    else editor_count_UP=0;
    if (EDITOR_BUTTON_DOWN_FUNC()) editor_count_DOWN+=1;
    else editor_count_DOWN=0;
    if (EDITOR_BUTTON_ENTER_FUNC()) editor_count_ENTER+=1;
    else editor_count_ENTER=0;
    if (EDITOR_BUTTON_ACCEPT_FUNC()) editor_count_ACCEPT+=1;
    else editor_count_ACCEPT=0;

    // for every change of editor mode you have to push ENTER
    // then you can change the next parameter
    if (editor_count_ENTER==1) {
        editor_mode+=1;
        editor_changed=1;
    }

    if (editor_mode>EDITOR_MAXMODE) editor_mode=1;

    // leave editor after last parameter
    editorShow(editor_mode); // show current editor on screen
    SREG=sreg;
    EDITOR_ENABLE_TIMER();
}



void editorStart(FILE *LCD) {
    editor_LCD=LCD;
    editor_count_UP=0;
    editor_count_DOWN=0;
    editor_count_ENTER=0;
    editor_count_ACCEPT=0;
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
    EDITOR_ENABLE_TIMER();
    sei();
    while (editor_enabled==1) {
    }

    EDITOR_DISABLE_TIMER();
    editor_enabled=0;
    if (lcd_load_default_glyphs_P!=NULL) {
        lcd_load_default_glyphs_P();
        lcd_home();
        lcd_clear();
    }
}



