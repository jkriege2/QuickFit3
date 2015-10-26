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

void editor_LED(uint8_t *value) {
    editor_uint8_turn(value, 0, led_count-1, 1, 1);
    configCalc();
}

void editor_uint8_turn(uint8_t *value, uint8_t min, uint8_t max, uint8_t incp, uint8_t fastinc_factor) {
    uint8_t inc=incp;
    //if ( (editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if ( (editor_count_UP > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) || (editor_count_DOWN > EDITOR_BUTTON_FASTFASTFASTEDITDELAY) ) inc=inc*fastinc_factor;
    if (editor_count_UP == EDITOR_BUTTON_FIRSTHIT
        || ((editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_UP % EDITOR_BUTTON_MOD==0))
        || ((editor_count_UP > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_UP % EDITOR_BUTTON_MOD==0))
        ) {
        *value+=inc;
        editor_changed=1;
        if (*value>max) *value=*value-max-1;
    }
    if (editor_count_DOWN == EDITOR_BUTTON_FIRSTHIT || ((editor_count_DOWN > EDITOR_BUTTON_FASTEDITDELAY) && (editor_count_DOWN % EDITOR_BUTTON_MOD==0))) {
        if (*value > min+inc) {
            *value-=inc;
            editor_changed=1;
        } else {
            *value=max+*value-inc+1;
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

void editor_restore(void) {
    if (editor_count_ACCEPT == EDITOR_BUTTON_FIRSTHIT) {
        configLoadStd();
        editor_changed=1;
        fprintf(editor_LCD, "\ndefaults restored           \r ... ... ... ...                 ");
        for (int i=0; i<4; i++) {
            PWM0_set(0);
            _delay_ms(125);
            PWM0_set(LCD_BACKLIGHT);
            _delay_ms(125);
        }
    }
}

void editor_quit(void) {
    if (editor_count_ACCEPT == EDITOR_BUTTON_FIRSTHIT) {
        editor_enabled=0;
        // wait some time (1 sec)
        fprintf(editor_LCD, "\fsaving ...      \n                ");
        editor_changed=1;
        configSave();
        for (int i=0; i<4; i++) {
            PWM0_set(0);
            _delay_ms(125);
            PWM0_set(LCD_BACKLIGHT);
            _delay_ms(125);
        }
    }
}


void editorShow(unsigned char emode) {
    char s[3];
    uint8_t show_updown=1;
    uint8_t noexit=0;



    switch(emode) {
    case 1:
        editor_uint8(&LCD_BACKLIGHT, 0, 255, 5, 2);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fLCD backlight\n = %3d", LCD_BACKLIGHT);
        PWM0_set(LCD_BACKLIGHT);
        break;
    case 2:
        editor_uint16(&FAN_START, 20, 50, 2, 2);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fFAN_START\n = %2d °C", FAN_START);
        break;
    case 3:
        editor_uint16(&FAN_FACTOR, 1, 20, 1, 1);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fFAN_FACTOR\n = %2d", FAN_FACTOR);
        break;
    case 4:
        editor_uint16(&FAN_OFFSET, 0, 255, 2, 2);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fFAN_OFFSET\n = %3d", FAN_OFFSET);
        break;
    case 5:
        editor_baudrate(&BAUDRATE);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fBAUDRATE\n = %ld", BAUDRATE);
        break;
    case 6: {
            editor_LED(&LED);
            if (LED>=led_count) { LED=0; editor_changed=1; }
            char n[20];
            strcpy_P(n, (PGM_P)pgm_read_word(&(led_names[LED])));
            if (editor_changed==1) fprintf(editor_LCD, "\fselect LED\n=%13s", n);
        }
        break;
    case 7:
        if (LED==0 || LED>=led_count) {
            editor_uint16(&CURRENT_MAX, 0, 2000, 1, 10);
            if (CURRENT_100>CURRENT_MAX) CURRENT_MAX=CURRENT_100;
            if (editor_changed==1) lcd_clear();
            if (editor_changed==1) fprintf(editor_LCD, "\fCURRENT_MAX\n = %d A", CURRENT_MAX);
        } else {
            show_updown=0;
            if (editor_changed==1) lcd_clear();
            if (editor_changed==1) fprintf(editor_LCD, "\fCURRENT_MAX\n = %d Afix", CURRENT_MAX);
        }
        break;
    case 8:
         if (LED==0 || LED>=led_count) {
            editor_uint16(&CURRENT_100, 0, CURRENT_MAX, 1, 5);
            if (editor_changed==1) lcd_clear();
            if (editor_changed==1) fprintf(editor_LCD, "\fCURRENT_100%%\n = %d A", CURRENT_100);
        } else {
            show_updown=0;
            if (editor_changed==1) lcd_clear();
            if (editor_changed==1) fprintf(editor_LCD, "\fCURRENT_100%%\n = %d Afix", CURRENT_100);
        }
        break;
    case 9:
        editor_uint16(&LEDBLINK, 0, 20000, 1, 10);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fLED blink f.\n = %4d Hz", LEDBLINK);
        break;
    case 10:
        editor_uint16(&IADC_MEASUREMENTS, 0, 99, 1, 2);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fi ADC #meas.\n = %2d", IADC_MEASUREMENTS);
        break;
    case 11:
        editor_uint16(&IADC_WAIT, 0, 9999, 1, 10);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fi ADC period\n = %4d µs", IADC_WAIT);
        break;
    case 12:
        editor_uint16(&EADC_MEASUREMENTS, 0, 99, 1, 2);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fe ADC #meas.\n = %2d", EADC_MEASUREMENTS);
        break;
    case 13:
        editor_uint16(&EADC_WAIT, 0, 9999, 1, 10);
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fe ADC period\n = %4d µs", EADC_WAIT);
        break;

    case EDITOR_MAXMODE-1:
        editor_restore();
        show_updown=0;
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\frestore def.\npress %c", EDITOR_ACCEPT_GLYPH);
        PWM0_set(LCD_BACKLIGHT);
        noexit=1;
        break;
    case EDITOR_MAXMODE:
        editor_quit();
        show_updown=0;
        if (editor_changed==1) lcd_clear();
        if (editor_changed==1) fprintf(editor_LCD, "\fDONE!!!         \npress %c          ", EDITOR_ACCEPT_GLYPH);
        noexit=1;
        break;
    }

    if (noexit!=1) editor_quit();


    if (editor_changed==1) {
        s[2]=0;
        s[0]=EDITOR_UP_GLYPH;
        if (show_updown==0) s[0]=' ';
        s[1]=EDITOR_ENTER_GLYPH;
        lcd_pos_string(14, 1, s);
        s[0]=EDITOR_DOWN_GLYPH;
        if (show_updown==0) s[0]=' ';
        //s[1]=' ';
        //if (emode>=EDITOR_MAXMODE-1 ) s[1]=EDITOR_ACCEPT_GLYPH;
        s[1]=EDITOR_ACCEPT_GLYPH;
        lcd_pos_string(14, 2, s);
    }
    editor_changed=0;
};




void editorButtonInt(void) {
    EDITOR_DISABLE_TIMER();
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
    sei();
    EDITOR_ENABLE_TIMER();
}



void editorStart(FILE *LCD) {
    editor_LCD=LCD;
    editor_count_UP=0;
    editor_count_DOWN=0;
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



