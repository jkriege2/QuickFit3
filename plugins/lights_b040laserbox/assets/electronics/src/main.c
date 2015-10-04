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


// FUSES:
//
//


#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <math.h>
#include "tools.h"
#include "lcd.h"
#include "lcd_myfont.h"
#include "uart.h"
#include "uart1.h"
#include "config.h"
#include "editor.h"
#include "version.h"
#include "rotencoder.h"

// update display ever ... cycles (15Hz cycles!)
#define DISPLAY_UPDATE 20
#define DISPLAY_UPDATEANIMATION 4
#undef SHOW_BLINKING

#define LFUSE         __attribute__ ((section ("lfuses")))
#define HFUSE         __attribute__ ((section ("hfuses")))
#define EFUSE         __attribute__ ((section ("efuses")))




// select ext crystal 3-8Mhz
unsigned char lfuse LFUSE = 0xFD;
unsigned char hfuse HFUSE = 0xD8; //0xDF;
unsigned char efuse EFUSE = 0xFF;






// variable declaration
FILE *LCD;
FILE* UART;
FILE* UART_LASER;

uint8_t ERROR_OCCURED;

volatile uint8_t button_ONOFF_state; // stores the state of ON/OFF in the last ISR call
volatile uint8_t button_MENU_state; // stores the state of MENU in the last ISR call
volatile uint8_t button_ROTENC_state; // how long are the buttons already pressed
volatile uint32_t time_counter=0; // wall clock with +15 per second
volatile uint8_t lasersend_timeout;
volatile uint8_t COMPA_counter;
volatile uint8_t LASER_ON=0;
volatile double POW_MEAS=0;
volatile double CUR_MEAS=0;
volatile uint8_t ERROR=0;
volatile uint8_t CURRENTLASER_USER=0;
volatile uint8_t SELECTED_LASER=0;

#ifdef TWO_LASERS
volatile uint8_t LASER_ON2=0;
volatile double POW_MEAS2=0;
volatile double CUR_MEAS2=0;
volatile uint8_t ERROR2=0;
#endif


// interrupt service routine for timer/counter1 compare A interrupt
ISR(TIMER1_COMPA_vect) {
    cli();
    ROTENCODER0_isr();
    COMPA_counter++;
    if (COMPA_counter>100) {
        COMPA_counter=0;
        time_counter++;
        lasersend_timeout++;
        if (time_counter>0xFFFFFFFA) time_counter=0;
        if (editor_enabled!=0) {
            editorButtonInt();
        } else {
            // auxiliary variables
            uint8_t a;

            // check menu button
            a=button_MENU();
            if (a!=0 && button_MENU_state==0) {
                editor_enabled=1;
            }
            button_MENU_state=a; // store button state

            // if ON/OFF is externally controlled we check the external line PB2
            // otherwise the ON/OFF button
            if (EXTERNAL_ONOFF_MODE==0) {
                // check on/off button
                a=button_ONOFF();
                if (a!=0 && button_ONOFF_state==0) {
                    if (LASER_ON==0) LASER_ON=1; else LASER_ON=0;
                }
                button_ONOFF_state=a; // store button state
            }

            // only if external intensity control is switched off, we look at the rotary encoder!
            if (EXTERNAL_INTENSITY_MODE==0) {
                // check rot encoder button
                a=ROTENCODER0_button();
                if (a!=0 && button_ROTENC_state==0) {
                    ERROR=0;
                }
                button_ROTENC_state=a; // store button state
            }
        }
    }
    sei();
}


void lcd_load_default_glyphs(void) {
    lcd_define_letter_P(1, LCDC_INV_ARROWUP);
    lcd_define_letter_P(2, LCDC_INV_ARROWDOWN);
    lcd_define_letter_P(3, LCDC_INV_ENTER);
    lcd_define_letter_P(4, LCDC_INV_ONOFF);
    /*lcd_define_letter_P(5, LCDC_FAN1);
    lcd_define_letter_P(6, LCDC_FAN2);
    lcd_define_letter_P(7, LCDC_FAN3);
    lcd_define_letter_P(8, LCDC_FAN4);*/
}











inline void select_laser(uint8_t laser) {
#ifdef TWO_LASERS
#  ifdef ENABLEDEBUGMODE
    if (DEBUG_MODE!=0) {
        fprintf_P(UART, PSTR("D> sel. laser %d\n"), (int)laser);
    }
    SELECTED_LASER=laser;
#  endif
#else
    SELECTED_LASER=0;
#endif
}


inline uint8_t laser_connected(void) {
    uint16_t adc=ADC_read(0, 4, 10);
#  ifdef ENABLEDEBUGMODE
    if (DEBUG_MODE!=0) {
        fprintf_P(UART, PSTR("D> is laser connected U=%d\n"), (int)adc);
    }
#  endif
    if (CHECK_LASERCONNECTED==0) return (1==1);
    return adc>200; // 200 ADC units ~ 0.5V
    // laser power output is connected to ADC via a 1:3.2 resistor voltage divider,
    // so 5V ~ 1.6V at ADC input
}




uint8_t str_startswith(char* data, char* startswith) {
    if (startswith==NULL) return 1;
    int swl=strlen(startswith);
    int dl=strlen(data);
    if (swl<=0) return 1;
    if (dl<swl) return 0;
    int i=0;
    for (i=0; i<swl; i++) {
        if (data[i]!=startswith[i]) return 0;
    }
    return 1;
}





#ifdef COHERENT
#  include "coherent.h"
#endif
#ifdef COBOLT
#  include "cobolt.h"
#endif



inline void laser_state(void) {
    select_laser(0);
    laser_stat(&ERROR, &POW_MEAS, &CUR_MEAS, CURRENTLASER_USER==SELECTED_LASER);
}

#ifdef TWO_LASERS
inline void laser_state2(void) {
    select_laser(1);
    laser_stat(&ERROR2, &POW_MEAS2, &CUR_MEAS2, CURRENTLASER_USER==SELECTED_LASER);
}
#endif

inline void laser_send_power(void) {
    select_laser(0);
    laser_send_pow(&ERROR, LASER_ON, POWER, CURRENTLASER_USER==SELECTED_LASER, &POW_MEAS);
}

#ifdef TWO_LASERS
inline void laser_send_power2(void) {
    select_laser(1);
    laser_send_pow(&ERROR2, LASER_ON2, POWER2, CURRENTLASER_USER==SELECTED_LASER, &POW_MEAS2);
}
#endif


inline void laser_init(void) {
    select_laser(0);
    laser_ini(&ERROR, &MAX_POWER, &WAVELENGTH);
}

#ifdef TWO_LASERS
inline void laser_init22(void) {
    select_laser(1);
    laser_init(&ERROR, &MAX_POWER2, &WAVELENGTH2);
}
#endif











// sends status information to UART
static inline void command_info(void) {
#ifdef TWO_LASERS
    fprintf_P(UART, PSTR("\nRS232 Double Laser Controller v%d.%d\n"), (int)MAJOR, (int)MINOR);
#else
    fprintf_P(UART, PSTR("\nRS232 Laser Controller v%d.%d\n"), (int)MAJOR, (int)MINOR);
#endif
    fprintf_P(UART, PSTR("  (c) %s.%s by Jan Krieger (DKFZ)\n"), MONTH, YEAR);
    uart1_puts_P(PSTR("  j.krieger@dkfz.de --  jan@jkrieger.de\n\n"));
}

// sends status information to UART
static inline void command_version(void) {
    fprintf(UART, "%s (%s.%s)\n\n", FULLVERSION_STRING, MONTH, YEAR);
}

// sends current laser power
inline void command_getpower(void) {
    laser_state();
    double p=POW_MEAS;
    fprintf(UART, "%f\n\n", p);
}
#ifdef TWO_LASERS
inline void command_getpower2(void) {
    laser_state2();
    double p=POW_MEAS2;
    fprintf(UART, "%f\n\n", p);
}
#endif

// sends current laser power
inline void command_getsetpower(void) {
    laser_state();
    double p=((double)POWER)/10.0;
    fprintf(UART, "%f\n\n", p);
}
#ifdef TWO_LASERS
inline void command_getsetpower2(void) {
    laser_state2();
    double p=((double)POWER2)/10.0;
    fprintf(UART, "%f\n\n", p);
}
#endif

// sends current laser power
inline void command_getwavelength(void) {
    laser_state();
    fprintf(UART, "%f\n\n", WAVELENGTH);
}
#ifdef TWO_LASERS
inline void command_getwavelength2(void) {
    laser_state2();
    fprintf(UART, "%f\n\n", WAVELENGTH2);
}
#endif

// sends current laser power
inline void command_getonoff(void) {
    laser_state();
    fprintf(UART, "%d\n\n", LASER_ON);
}
#ifdef TWO_LASERS
inline void command_getonoff2(void) {
    laser_state2();
    fprintf(UART, "%d\n\n", LASER_ON2);
}
#endif

// sends current laser current
inline void command_getcurrent(void) {
    laser_state();
    fprintf(UART, "%f\n\n", CUR_MEAS);
}
#ifdef TWO_LASERS
inline void command_getcurrent2(void) {
    laser_state();
    fprintf(UART, "%f\n\n", CUR_MEAS2);
}
#endif

// sends current laser current
inline void command_getADC4(void) {
    fprintf(UART, "%f\n\n", (double)ADC_read(2, 10, 10)/1024.0*2.56);
}

// sends current laser current
inline void command_getADC6(void) {
    fprintf(UART, "%f\n\n", (double)ADC_read(3, 10, 10)/1024.0*2.56);
}

// sends current laser current
inline void command_set_digIO5(void) {
    DDRB|=0b00010000;
    int c=0;
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='0')&&(c!='1')) {
        c=uart1_getc_nowait();
    }
    if (time_counter<timeout) { // process instruction if there was no timeout
        if (c=='0') {
            cbi(PORTB, PB4);
        } else if (c=='1') {
            sbi(PORTB, PB4);
        } else uart1_puts_P(PSTR("ERROR: INVALID DATA\n\n"));
    } else uart1_puts_P(PSTR("ERROR: TIMEOUT\n\n"));
}

// sends current laser current
inline void command_getADClaser(void) {
    fprintf(UART, "%f\n\n", (double)ADC_read(0, 10, 10)/1024.0*2.56);
}

void command_getstate(uint8_t laser) {
    uint8_t err;
    if (laser==0) {
        laser_state();
        err=ERROR;
#ifdef TWO_LASERS
    } else if (laser==1) {
        laser_state2();
        err=ERROR2;
#endif
    }
    fprintf(UART, "%d: ", err);
    switch (err) {
        case 0: uart1_puts_P(PSTR("OK\n\n"));; break;
        case 1: uart1_puts_P(PSTR("BAD INSTRUCTION ERROR\n\n")); break;
        case 2: uart1_puts_P(PSTR("LASER RECEIVE TIMEOUT\n\n")); break;
        case 3: uart1_puts_P(PSTR("TEMPERATURE ERROR\n\n")); break;
        case 4: uart1_puts_P(PSTR("INTERLOCK OPEN\n\n")); break;
        case 5: uart1_puts_P(PSTR("CONSTANT POWER FAULT\n\n")); break;
        case 6: uart1_puts_P(PSTR("LASER NOT CONNECTED\n\n")); break;
        default: uart1_putc('\n'); uart1_putc('\n'); break;
    }
}




// read a power (<=6 digits) from UART and set the laser power to this value
void command_setpower(uint8_t laser) {
    char s[10]= {0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<=6)) {
        c=uart1_getc_nowait();
        if ((c>='0' && c<='9')||(c=='+')||(c=='-')||(c=='.')) {
            s[i]=c;
            i++;
        }

    }

    if (time_counter<timeout) { // process instruction if there was no timeout
        char *e;
        double cuf=strtod(s, &e);
        uint16_t cu=round(cuf*10.0);
        if (laser==0) {
            if (*e=='\0' && POWER!=cu) {
                POWER=cu;
                configSaveNonMenu();
                laser_send_power();
            } else if (*e!='\0') uart1_puts_P(PSTR("ERROR: INVALID DATA\n\n"));
#ifdef TWO_LASERS
        } else if (laser==2) {
            if (*e=='\0' && POWER2!=cu) {
                POWER2=cu;
                configSaveNonMenu();
                laser_send_power2();
            } else if (*e!='\0') uart1_puts_P(PSTR("ERROR: INVALID DATA\n\n"));
#endif
        }
    } else uart1_puts_P(PSTR("ERROR: TIMEOUT\n\n"));
}


// switch laser on (1) or off (0)
void command_setonoff(uint8_t laser) {
    char s[10]= {0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<=2)) {
        c=uart1_getc_nowait();
        if ((c>='0' && c<='1')) {
            s[i]=c;
            i++;
        }

    }

    if (time_counter<timeout) { // process instruction if there was no timeout
        //char *e;
        if (laser==0) {
            LASER_ON = (s[0]!='0')?1:0;
            laser_send_power();
#ifdef TWO_LASERS
        } else if (laser==1) {
            LASER_ON2 = (s[0]!='0')?1:0;
            laser_send_power2();
#endif
        }
    } else uart1_puts_P(PSTR("ERROR: TIMEOUT\n\n"));
}

#ifdef HAS_COMMAND_SETPARAMS
// set the controller parameters P<NAME>=<WERT>
void command_setparameter(void) {
    char sn[31]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char sv[21]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char errinv[]="ERROR: INVALID DATA\n\n";
    int i=0;
    int c=0;
    char printcommand=1;
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='=') && (c!='\n') && (c!='\r') && (i<29)) {
        c=uart1_getc_nowait();
        if ((c>='0' && c<='9')
            ||((c>='A') && (c<='Z'))
            ||((c>='a') && (c<='z'))
            ||(c=='_')) {

            sn[i]=c;
            i++;
        }
    }
    if (c=='=' && time_counter<timeout) { // only go on when the instruction format is correct
        i=0;
        c=0;
        timeout=time_counter+15;
        while ((time_counter<timeout) && (c!='=') && (c!='\n') && (c!='\r') && (i<19)) {
            c=uart1_getc_nowait();
            if ((c>='0' && c<='9')
                ||((c>='A') && (c<='Z'))
                ||((c>='a') && (c<='z'))
                ||(c=='_')||(c==' ')||(c=='.')||(c==',')||(c==':')||(c==';')||(c=='/')||(c=='\\')||(c=='+')||(c=='*')||(c=='#')||(c=='-')) {

                sv[i]=c;
                i++;
            }
        }
        if (time_counter<timeout) printcommand=0;
    }
    // TODO: use strcmp_P() here???
    if (time_counter<timeout) { // process instruction if there was no timeout
        if (printcommand==1) {
            if (strcmp_P(sn, PSTR("LCD_BACKLIGHT"))==0) {
                fprintf(UART, "%d\n\n", (int)LCD_BACKLIGHT);
            } else if (strcmp_P(sn, PSTR("LCD_CONTRAST"))==0) {
                fprintf(UART, "%d\n\n", (int)LCD_CONTRAST);
            } else if (strcmp_P(sn, PSTR("BAUDRATE"))==0) {
                fprintf(UART, "%ld\n\n", (long int)BAUDRATE);
            } else if (strcmp_P(sn, PSTR("MAX_POWER"))==0) {
                fprintf(UART, "%ld\n\n", (long int)MAX_POWER);
#ifdef ENABLEDEBUGMODE
            } else if (strcmp_P(sn, PSTR("DEBUG_MODE"))==0) {
                if (DEBUG_MODE>0) uart1_puts("1\n\n"); else uart1_puts("0\n\n");
#endif
            } else if (strcmp_P(sn, PSTR("EXTINTENSITY_ENABLE"))==0) {
                if (EXTERNAL_INTENSITY_MODE>0) uart1_puts("1\n\n"); else uart1_puts("0\n\n");
            } else if (strcmp_P(sn, PSTR("EXTONOFF_ENABLE"))==0) {
                if (EXTERNAL_ONOFF_MODE>0) uart1_puts("1\n\n"); else uart1_puts("0\n\n");
            } else if (strcmp_P(sn, PSTR("EXTINTENSITY_MAX"))==0) {
                fprintf(UART, "%ld\n\n", (long int)EXTINTENSITY_MAX);
            } else if (strcmp_P(sn, PSTR("EXTINTENSITY_MIN"))==0) {
                fprintf(UART, "%ld\n\n", (long int)EXTINTENSITY_MIN);
            } else if (strcmp_P(sn, PSTR("WAVELENGTH"))==0) {
                fprintf(UART, "%f\n\n", (double)WAVELENGTH);
            } else {
                uart1_puts_P(PSTR("ERROR: UNKNOWN PARAMETER '"));
                uart1_puts(sn);
                uart1_puts_P(PSTR("'\n\n"));
            }
        } else {
            if (strcmp_P(sn, PSTR("LCD_BACKLIGHT"))==0) {
                char *e;
                uint16_t v=strtoul(sv, &e, 10);
                if (*e=='\0') LCD_BACKLIGHT=v; else uart1_puts(errinv);
            } else if (strcmp_P(sn, PSTR("LCD_CONTRAST"))==0) {
                char *e;
                uint32_t v=strtoul(sv, &e, 10)      ;
                if (*e=='\0') LCD_CONTRAST=v; else uart1_puts(errinv);
            } else if (strcmp_P(sn, PSTR("BAUDRATE"))==0) {
                char *e;
                uint16_t v=strtoul(sv, &e, 10);
                if (*e=='\0') BAUDRATE=v; else uart1_puts(errinv);
            } else if (strcmp_P(sn, PSTR("MAX_POWER"))==0) {
                char *e;
                uint16_t v=strtoul(sv, &e, 10);
                if (*e=='\0') MAX_POWER=v; else uart1_puts(errinv);
#ifdef ENABLEDEBUGMODE
            } else if (strcmp_P(sn, PSTR("DEBUG_MODE"))==0) {
                char *e;
                uint16_t v=strtoul(sv, &e, 10);
                if (*e=='\0') DEBUG_MODE=(v>0)?1:0; else uart1_puts(errinv);
#endif
            } else if (strcmp_P(sn, PSTR("EXTINTENSITY_ENABLE"))==0) {
                char *e;
                uint16_t v=strtoul(sv, &e, 10);
                if (*e=='\0') EXTERNAL_INTENSITY_MODE=(v>0)?1:0; else uart1_puts(errinv);
            } else if (strcmp_P(sn, PSTR("EXTONOFF_ENABLE"))==0) {
                char *e;
                uint16_t v=strtoul(sv, &e, 10);
                if (*e=='\0') EXTERNAL_ONOFF_MODE=(v>0)?1:0; else uart1_puts(errinv);
            } else if (strcmp_P(sn, PSTR("EXTINTENSITY_MAX"))==0) {
                char *e;
                uint16_t v=strtoul(sv, &e, 10);
                if (*e=='\0') EXTINTENSITY_MAX=v; else uart1_puts(errinv);
            } else if (strcmp_P(sn, PSTR("EXTINTENSITY_MIN"))==0) {
                char *e;
                uint16_t v=strtoul(sv, &e, 10);
                if (*e=='\0') EXTINTENSITY_MIN=v; else uart1_puts(errinv);
            } else if (strcmp_P(sn, PSTR("WAVELENGTH"))==0) {
                char *e;
                double v=strtod(sv, &e);
                if (*e=='\0') WAVELENGTH=v; else uart1_puts(errinv);
            } else {
                uart1_puts_P(PSTR("ERROR: UNKNOWN PARAMETER '"));
                uart1_puts(sn);
                uart1_puts_P(PSTR("'\n\n"));
            }
        }
    } else uart1_puts_P(PSTR("ERROR: TIMEOUT\n\n"));
    configSave();
}
#endif




int main(void) {

    lederror_off();
    ledlock_off();
    ledpow_off();
    ledlaser_off();

    ERROR_OCCURED=0;
	LASER_ON=0;
#ifdef TWO_LASERS
	LASER_ON2=0;
#endif

    editor_enabled=0;
    button_MENU_state=1;
    lcd_load_default_glyphs_P=lcd_load_default_glyphs;
    cli();

    configLoad();

    InitHardware();
    ROTENCODER0_init();

    PWM2A_enable();
    PWM2A_set(LCD_BACKLIGHT);

    lcd_init(0, 0);
    lcd_load_default_glyphs();
    LCD=fdevopen(lcd_write, NULL);          //LCD-Ausgabe initialisieren

    uart_init(BAUDRATE_LASER, 8, 1, UART_NOPARITY);
    UART_LASER=fdevopen(uart_putchar, uart_getchar);


    uart1_init(BAUDRATE, 8, 1, UART_NOPARITY);
    UART=fdevopen(uart1_putchar, uart1_getchar);
    command_info();

    lcd_setcursor(1, 0, 0);
    lcd_setdoubleheight(1);
    fprintf(LCD, "\fLASERBOX v%ld.%ld     \n (c)%s.%s JK          ", MAJOR, MINOR, MONTH, YEAR);
    //_delay_ms(2000);
    //char cf=0;
    #ifdef SHOW_BLINKING
    double delay=5;
    for (cf=0; cf<1; cf++) {
		ledlaser_on();
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay);
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay*3);
		ledlaser_off();


		ledpow_on();
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay);
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay);
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay*3);
		ledpow_off();

 		lederror_on();
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay);
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay);
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay*3);
		lederror_off();

		ledlock_on();
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay);
        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
        _delay_ms(delay*3);
		ledlock_off();

        PWM2A_set(255);
        _delay_ms(delay);
        PWM2A_set(0);
    }
    #endif


    lcd_setdoubleheight(0);
    button_ONOFF_state=button_ONOFF();
    button_MENU_state=button_MENU();


    timer1A_enable(1000);
    sei();



    fprintf(LCD, "\fLASERBOX v%ld.%ld     \n init laser ...       ", MAJOR, MINOR);
    laser_init();
    laser_send_power();
#ifdef TWO_LASERS
    laser_init2();
    laser_send_power2();
#endif
    lcd_clear();
    lcd_home();
    fprintf(LCD, "\v \x01\x02    \x04  MENU   ");

    lederror_off();
    ledlock_off();
    ledpow_off();
    ledlaser_off();

	uint8_t laser_on_old=LASER_ON+1;
#ifdef TWO_LASERS
	uint8_t laser_on_old2=LASER_ON2+1;
#endif
	uint8_t laser_connected_old=laser_connected()+1;

	//uint32_t resend_laser_settings_timeout=time_counter+30;

    while(1) {

		uint8_t lc=laser_connected();

		// EXTERNAL LASER ON/OFF MODE:
		if (EXTERNAL_ONOFF_MODE!=0) {
            LASER_ON=(line_ONOFFFINPUT())?1:0;
#ifdef TWO_LASERS
            LASER_ON2=(line_ONOFFFINPUT())?1:0;
#endif
        }

        if (!lc) {
            LASER_ON=0;
#ifdef TWO_LASERS
            LASER_ON2=0;
#endif
        }

        // if laser has been switched
		if ((laser_connected_old!=lc) && lc) {
			LASER_ON=0;
			laser_on_old=1;
#ifdef TWO_LASERS
			LASER_ON2=0;
			laser_on_old2=1;
#endif
            fprintf_P(LCD, PSTR("\f                 \n init laser ...       "));
			_delay_ms(100);
            laser_init();
#ifdef TWO_LASERS
            laser_init2();
#endif
			_delay_ms(100);
			laser_send_power();
#ifdef TWO_LASERS
			laser_send_power2();
#endif
		}
		laser_connected_old=lc;


		// EXTERNAL LASER INTENSITY MODE:
		if (EXTERNAL_INTENSITY_MODE!=0) {
            int32_t ad=ADC_read(2, 4, 10)*25/10;
            if (ad<EXTINTENSITY_MIN) POWER=0;
            else if (ad>EXTINTENSITY_MAX) POWER=MAX_POWER*10;
            else {
                POWER=EXTINTENSITY_FACTOR*(ad-EXTINTENSITY_MIN)/EXTINTENSITY_RANGE;
                laser_send_power();
#ifdef TWO_LASERS
                POWER2=EXTINTENSITY_FACTOR*(ad-EXTINTENSITY_MIN)/EXTINTENSITY_RANGE;
                laser_send_power2();
#endif
            }

		} else {
#ifdef TWO_LASERS
            int16_t rec=ROTENCODER0_read()*1;
            if (!ROTENCODER0_button()) rec=rec*10;

            if (CURRENTLASER_USER==0) {
                POWER-=rec;
                if (POWER<0) POWER=0;
                if (POWER>MAX_POWER*10) POWER=MAX_POWER*10;

                if ((rec!=0) || (laser_on_old!=LASER_ON))  {
                    ERROR=0;
                    laser_send_power();
                    configSaveNonMenu();
                }

            } else {
                POWER2-=rec;
                if (POWER2<0) POWER2=0;
                if (POWER2>MAX_POWER2*10) POWER2=MAX_POWER2*10;

                if ((rec!=0) || (laser_on_old2!=LASER_ON2))  {
                    ERROR2=0;
                    laser_send_power2();
                    configSaveNonMenu();
                }
           }

#else
            int16_t rec=ROTENCODER0_read()*1;
            if (!ROTENCODER0_button()) rec=rec*10;
            POWER-=rec;
            if (POWER<0) POWER=0;
            if (POWER>MAX_POWER*10) POWER=MAX_POWER*10;

            if ((rec!=0) || (laser_on_old!=LASER_ON))  {
                ERROR=0;
                laser_send_power();
                configSaveNonMenu();
            }
#endif

		}


        // make sure laser power is set at least 3-times per second
        if (lasersend_timeout>3) {
            laser_send_power();
#ifdef TWO_LASERS
            laser_send_power2();
            lasersend_timeout=0;
#endif
        }

		laser_on_old=LASER_ON;
        laser_state();
#ifdef TWO_LASERS
		laser_on_old2=LASER_ON2;
        laser_state2();
#endif



#ifdef TWO_LASERS
        char clchar=' ';
        if(CURRENTLASER_USER==0) clchar='*';
        if (LASER_ON==1)
            fprintf(LCD, "\f%cP=%5.1f|%5.1fmW ", clchar, (float)POWER/10.0, POW_MEAS);
        else
            fprintf(LCD, "\f%cPset=%5.1f mW       ", clchar, (float)POWER/10.0);
        if (ERROR==0) {
            if (CURRENTLASER_USER==0) lederror_off();
        } else {
            fprintf(LCD, "\f%c%s", clchar, laser_errordescription(ERROR));
            if (CURRENTLASER_USER==0) lederror_on();
        }
        clchar=' ';
        if(CURRENTLASER_USER==1) clchar='*';
        if (LASER_ON2==1)
            fprintf(LCD, "\n%cP=%5.1f|%5.1fmW ", clchar, (float)POWER2/10.0, POW_MEAS2);
        else
            fprintf(LCD, "\n%cPset=%5.1f mW       ", clchar, (float)POWER2/10.0);

        if (ERROR2==0) {
            if (CURRENTLASER_USER==1) lederror_off();
        } else {
            fprintf(LCD, "\n%c%s", clchar, laser_errordescription(ERROR2));
            if (CURRENTLASER_USER==1) lederror_on();
        }

#else
        if (LASER_ON==1)
            fprintf(LCD, "\fP=%5.1fmW        ", (float)POWER/10.0);
        else
            fprintf(LCD, "\fPset=%5.1fmW      ", (float)POWER/10.0);

        if (ERROR==0) {
            if (CURRENTLASER_USER==0) lederror_off();
            fprintf(LCD, "\nP=%6.2f %s", POW_MEAS, laser_getadditionalinfo());
        } else {
            fprintf(LCD, "\f%s ", laser_errordescription(ERROR));
            if (CURRENTLASER_USER==0) lederror_on();
        }
#endif


        //fprintf(LCD, "\v \x02\x01    \x04  MENU   ");
        fprintf(LCD, "\v          MENU   ");
        if (EXTERNAL_INTENSITY_MODE==0) { lcd_pos_string(1,3, "\x02\x01"); }
        else { lcd_pos_string(0,3, "EXT\x02\x01"); }
        if (EXTERNAL_ONOFF_MODE==0) { lcd_pos_string(7,3, "\x04"); }
        else { lcd_pos_string(5,3, "EXT\x04"); }



        // now we distinguish between editor and non-editor mode
        if (editor_enabled==1) {
            //lcdback_on();
            editorStart(LCD);
            uart1_init(BAUDRATE, 8, 1, UART_NOPARITY);
            _delay_ms(50);
            //UART=fdevopen(uart1_putchar, uart1_getchar);
            //command_info();
            lcdback_off();
            LCD_BACKLIGHT+=10;
            PWM2A_set(LCD_BACKLIGHT);
            editor_enabled=0;
            lcd_clear();
            lcd_home();
            laser_init();
#ifdef TWO_LASERS
            laser_init2();
#endif
        } else {

            // NON-EDITOR/STANDARD MODE

            // set LCD backligh intensity
            PWM2A_set(LCD_BACKLIGHT);
            lcd_setcontrast(LCD_CONTRAST);

        }


        int c=uart1_getc_nowait();

#ifdef ENABLEDEBUGMODE
#  ifndef LIGHTWEIGHTSEBUGMODE
        if (DEBUG_MODE!=0) {
            fprintf(UART, "D> recv char: %d ('%c')\n", c, (char)c);
        }
#  endif
#endif
        switch (c) {

        case '?':
            command_info();
            break;
#ifdef HAS_COMMAND_SETPARAMS
        case 'P':
            command_setparameter();
            break;
#endif
        case 'p':
            command_setpower(0);
            break;
        case 'O':
            command_setonoff(0);
            break;
        case 'o':
            command_getonoff();
            break;
        case 'r':
            command_getpower();
            break;
        case 'w':
            command_getwavelength();
            break;
        case 'g':
            command_getsetpower();
            break;
        case 'i':
            command_getcurrent();
            break;
        case 'd':
            command_set_digIO5();
            break;
        case 's':
            command_getstate(0);
            break;
        case '4':
            command_getADC4();
            break;
        case '6':
            command_getADC6();
            break;
        case 'l':
            command_getADClaser();
            break;
        case 'V':
            command_version();
            break;

#ifdef TWO_LASERS
        case 'z':
            command_getstate(1);
            break;
        case 'Y':
            command_setpower(1);
            break;
        case 'X':
            command_setonoff(1);
            break;
        case 'x':
            command_getonoff2();
            break;
        case 'u':
            command_getpower2();
            break;
        case 'q':
            command_getwavelength2();
            break;
        case 'y':
            command_getsetpower2();
            break;
        case 't':
            command_getcurrent2();
            break;

#endif
        default:
        case -1:
            break;
        }

    }

    return 0;
}
