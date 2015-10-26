
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "tools.h"
#include "lcd.h"
#include "lcd_myfont.h"
#include "uart.h"
#include "config.h"
#include "editor.h"
#include "version.h"
#include "i2c.h"


// update display ever ... cycles (15Hz cycles!)
#define DISPLAY_UPDATE 5
#define LED_EEPROM_UPDATE 5
#define DISPLAY_UPDATEANIMATION 4


// variable declaration
FILE *LCD;
FILE* UART;

volatile uint8_t current_on; // stores whether curent is switched on or off
volatile uint8_t button_ONOFF_state; // stores the state of ON/OFF in the last ISR call
volatile uint8_t button_MENU_state; // stores the state of MENU in the last ISR call
volatile uint8_t button_UPDOWN_length; // how long are the buttons already pressed
volatile uint8_t fan_animation_state=0; // how long are the buttons already pressed
volatile uint8_t fan_animation_counter=0; // how long are the buttons already pressed
volatile uint32_t time_counter=0; // wall clock with +15 per second
volatile char gc;
volatile uint8_t pwmfan=0;
volatile uint16_t current_measured=0;
volatile int16_t temperature;
volatile uint16_t blinkcounter=0;
char LED_NAME[20];

// interrupt service routine for timer/counter1 compare A interrupt
ISR(TIMER1_COMPB_vect) {
    editorButtonInt();
}

// interrupt service routine blinking LED mode
// TIMER 0 @ 16MHz / 256 = 62,5kHz overflow rate
ISR(TIMER0_OVF_vect) {
    blinkcounter++;
    if (blinkcounter>=LEDBLINK_compare) {
        ledcurrent_toggle();
        blinkcounter=0;
    }
}

// interrupt service routine for timer/counter1 compare A interrupt
ISR(TIMER1_COMPA_vect) {
    time_counter++;
    // auxiliary variables
    uint8_t a;
    uint8_t b;



    // check menu button
    a=button_MENU();
    if (a!=0 && button_MENU_state==0) {
        editor_enabled=1;
    }
    button_MENU_state=a; // store button state


    /*
      if parameter editor is active we do not change the LED settings,
      but we only call the editor's interrupt routine!
    */
    if (editor_enabled==0) {

        // switch output current on/off, depending on the variable current_on
        // and update current_on depending on button click
        a=button_ONOFF();
        if (a==0 && button_ONOFF_state!=0) {
            current_on=!current_on; // toggle current_on
        }
        button_ONOFF_state=a; // store button state

        // evaluate UP/DOWN buttons to set the CURRENT on the output
        a=button_UP();
        b=button_DOWN();
        if (b==0) {
            if (button_UPDOWN_length>40) {
                if (CURRENT>=20) {
                    CURRENT=CURRENT-20;
                    configSaveNonMenu();
                } else {
                    CURRENT=0;
                    configSaveNonMenu();
                }
            } else if (button_UPDOWN_length>20) {
                if (CURRENT>=10) {
                    CURRENT=CURRENT-10;
                    configSaveNonMenu();
                } else {
                    CURRENT=0;
                    configSaveNonMenu();
                }
            } else {
                button_UPDOWN_length++;
                if (CURRENT>=1 && (button_UPDOWN_length%2==0)) {
                    CURRENT=CURRENT-1;
                    configSaveNonMenu();
                }
            }
        } else if (a==0) {
            if (button_UPDOWN_length>40) {
                if (CURRENT<=CURRENT_MAX-20) {
                    CURRENT=CURRENT+20;
                    configSaveNonMenu();
                } else {
                    CURRENT=CURRENT_MAX;
                    configSaveNonMenu();
                }
            } else if (button_UPDOWN_length>20) {
                if (CURRENT<=CURRENT_MAX-10) {
                    CURRENT=CURRENT+10;
                    configSaveNonMenu();
                } else {
                    CURRENT=CURRENT_MAX;
                    configSaveNonMenu();
                }
            } else {
                button_UPDOWN_length++;
                if (CURRENT<=CURRENT_MAX-1 && (button_UPDOWN_length%2==0)) {
                    CURRENT=CURRENT+1;
                    configSaveNonMenu();
                }
            }
        } else {
            button_UPDOWN_length=0;
        }

    }



}

void lcd_load_default_glyphs(void) {
    lcd_define_letter_P(1, LCDC_INV_ARROWUP);
    lcd_define_letter_P(2, LCDC_INV_ARROWDOWN);
    lcd_define_letter_P(3, LCDC_INV_ENTER);
    lcd_define_letter_P(4, LCDC_INV_ONOFF);
    lcd_define_letter_P(5, LCDC_FAN1);
    lcd_define_letter_P(6, LCDC_FAN2);
    lcd_define_letter_P(7, LCDC_FAN3);
    lcd_define_letter_P(8, LCDC_FAN4);
}


// sends status information to UART
void command_info(void) {
    fprintf(UART, "\n---=== PCCS  v%s ===---\n", FULLVERSION_STRING);
    uart_puts(" Programmable Constant Current Source (for high power LEDs)\n");
    fprintf(UART, " (c) %s.%s by Jan Krieger (DKFZ)\n", MONTH, YEAR);
    uart_puts(" j.krieger@dkfz.de --  jan@jkrieger.de\n\n");
}

// sends status information to UART
void command_version(void) {
    fprintf(UART, "%s (%s.%s)\n\n", FULLVERSION_STRING, MONTH, YEAR);
}

// sends comma separated values of the
//         set_current [mA], measured_current [mA], LED_state [0/1], temperature [°C], fan speed [0..255], analog_input [mV]
void command_state(void) {
    fprintf(UART, "%d, %d, %c, %d, %d, %d\n\n", CURRENT, current_measured, (current_on==0)?'0':'1', temperature, pwmfan, extADC_read(EADC_MEASUREMENTS, EADC_WAIT));
}

// return the volatge at the external analog input in mV
void command_analogin(void) {
    fprintf(UART, "%d\n\n", extADC_read(EADC_MEASUREMENTS, EADC_WAIT));
}

// return the set_current [mA]
void command_get_setcurrent(void) {
    fprintf(UART, "%d\n\n", CURRENT);
}

// return the set_current [mA]
void command_get_measuredcurrent(void) {
    fprintf(UART, "%d\n\n", current_measured);
}

// return the measured current [mA]
void command_get_temperature(void) {
    fprintf(UART, "%d\n\n", temperature);
}

// return the LED
void command_get_led(void) {
    if (LED==0) {
        fprintf(UART, "%s\n\n", LED_NAME);
    } else {
        char n[20];
        strcpy_P(n, (PGM_P)pgm_read_word(&(led_names[LED])));
        fprintf(UART, "%s\n\n", n);
    }
}

// return the max. LED current [mA]
void command_get_max_current(void) {
    fprintf(UART, "%d\n\n", CURRENT_MAX);
}

// return whether the LED is on or off
void command_get_ledonoff(void) {
    fprintf(UART, "%c\n\n", (current_on==0)?'0':'1');
}
// return the 100% LED current [mA]
void command_get_100perc_current(void) {
    fprintf(UART, "%d\n\n", CURRENT_100);
}

// read a current (<4 digits) from UART and set the current to this value
void command_setcurrent(void) {
    char s[10]= {0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<4)) {
        c=uart_getc_nowait();
        if ((c>='0' && c<='9')||(c=='+')||(c=='-')) {
            s[i]=c;
            i++;
        }

    }

    if (time_counter<timeout) { // process instruction if there was no timeout
        char *e;
        uint16_t cu=strtoul(s, &e, 10);
        if (*e=='\0' && CURRENT!=cu) {
            CURRENT=cu;
            configSaveNonMenu();
        } else uart_puts("ERROR: INVALID DATA\n\n");
    } else uart_puts("ERROR: TIMEOUT\n\n");

}

// M0<pulselength in usecs>   switch on the LED for the specified number of microseconds
// M1<pulselength in usecs>   switch on the LED and the TTL output for the specified number of microseconds
void command_uspulse(void) {
    char ttl=0;
    char s[10]= {0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<8)) {
        c=uart_getc_nowait();
        if (ttl==0 && (c=='0' || c=='1')) ttl=c;
        if ((ttl!=0)&&(c>='0' && c<='9')) {
            s[i]=c;
            i++;
        }

    }
    char ok=0;
    uint16_t time=0;
    if (time_counter<timeout) { // process instruction if there was no timeout
        if (ttl=='0' || ttl=='1') {
            char *e;
            uint16_t cu=strtoul(s, &e, 10);
            if (*e=='\0' && CURRENT!=cu) {
                time=cu;
                ok=1;
            } else uart_puts("ERROR: INVALID DATA\n\n");
        } else uart_puts("ERROR: INVALID INSTRUCTION (M0<...> or M1<...> allowed)\n\n");
    } else uart_puts("ERROR: TIMEOUT\n\n");

    if (ok!=0) {
        if (ttl=='0') {
            cli();
            ledcurrent_on();
            _delay_us(time);
            ledcurrent_off();
            sei();
        } else {
            cli();
            ttl_on();
            ledcurrent_on();
            _delay_us(time);
            ledcurrent_off();
            ttl_off();
            sei();
        }
    }
}


// M0<pulselength in msecs>   switch on the LED for the specified number of milliseconds
// M1<pulselength in msecs>   switch on the LED and the TTL output for the specified number of milliseconds
void command_mspulse(void) {
    char ttl=0;
    char s[10]= {0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<8)) {
        c=uart_getc_nowait();
        if (ttl==0 && (c=='0' || c=='1')) ttl=c;
        if ((ttl!=0)&&(c>='0' && c<='9')) {
            s[i]=c;
            i++;
        }

    }
    char ok=0;
    uint16_t time=0;
    if (time_counter<timeout) { // process instruction if there was no timeout
        if (ttl=='0' || ttl=='1') {
            char *e;
            uint16_t cu=strtoul(s, &e, 10);
            if (*e=='\0' && CURRENT!=cu) {
                time=cu;
                ok=1;
            } else uart_puts("ERROR: INVALID DATA\n\n");
        } else uart_puts("ERROR: INVALID INSTRUCTION (M0<...> or M1<...> allowed)\n\n");
    } else uart_puts("ERROR: TIMEOUT\n\n");

    if (ok!=0) {
        if (ttl=='0') {
            cli();
            ledcurrent_on();
            _delay_ms(time);
            ledcurrent_off();
            sei();
        } else {
            cli();
            ttl_on();
            ledcurrent_on();
            _delay_ms(time);
            ledcurrent_off();
            ttl_off();
            sei();
        }
    }
}

void command_dump_eeprom(void) {
    uart_puts("EEPROM-CONTENTS:\n");
    uint8_t i=0, ok=0;
    for (i=0; i<255; i++) {
        uint8_t d=twi_24c02_readbyte(i, &ok);
        fprintf(UART, "%2X(%d) ",(int)d,(int)ok);
    }
    uart_putc('\n');
    for (i=0; i<255; i++) {
        uint8_t d=twi_24c02_readbyte(i, &ok);
        fprintf(UART, "%c(%d) ",(char)d,(int)ok);
    }
    uart_puts("\n\n");
}

void write_string_to_eeprom(uint8_t adress, uint8_t max_bytes) {
    int i=0;
    int c=0;
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!=',') && (c!='\n') && (c!='\r') && (i<max_bytes)) {
        c=uart_getc_nowait();
        if ((c>=32) && (c!='\n') && (c!='\r') && (i<max_bytes)) {
            twi_24c02_writebyte(adress+i, c);
            i++;
        }
    }
}

void write_string_from_eeprom_to_uart(uint8_t adress, uint8_t max_bytes) {
    uint8_t i=0;
    int c=1;
    if (bit_is_set(PINA, PA7)) { // check whether an EEPROM is connected (i.e. PA7 is pulled to 1
        if ((twi_24c02_readbyte(0,NULL)==0xAF) && (twi_24c02_readbyte(1,NULL)==0xFE)) {
            while ((c!=0) && (i<max_bytes)) {
                c=twi_24c02_readbyte(adress+i, NULL);
                if (c!=0) {
                    uart_putc(c);
                }
                i++;
            }
        }
    }
}


void command_get_power(void) {
    write_string_from_eeprom_to_uart(20, 20);
    uart_puts("\n\n");
}

void command_set_power(void) {
    write_string_to_eeprom(20, 20);
}

void command_get_manufacturer(void) {
    write_string_from_eeprom_to_uart(40, 20);
    uart_puts("\n\n");
}

void command_set_manufacturer(void) {
    write_string_to_eeprom(40, 20);
}

void command_get_wavelength(void) {
    write_string_from_eeprom_to_uart(60, 20);
    uart_puts("\n\n");
}

void command_set_wavelength(void) {
    write_string_to_eeprom(60, 20);
}


// program an attatched EEPROM LED_NAME[<=10chars],max_current_mA[<=4char],100%_current_mA[<=4chars]
void command_write_eeprom(void) {
    char sn[11]= {0,0,0,0,0,0,0,0,0,0,0};
    char svm[6]= {0,0,0,0,0,0};
    char svp[6]= {0,0,0,0,0,0};
    int i=0;
    int c=0;
    uint32_t timeout=time_counter+15;
    char ok=0;
    while ((time_counter<timeout) && (c!=',') && (c!='\n') && (c!='\r')) {
        c=uart_getc_nowait();
        if ((c>=32) && (c!=',') && (c!='\n') && (c!='\r') && (i<10)) {
            sn[i]=c;
            //fprintf(UART, "read sn %c (%d) @ %d\n", c,(int)c, i);
            i++;
        }
    }
    if (c==',' && time_counter<timeout) { // only go on when the instruction format is correct
        i=0;
        c=0;
        timeout=time_counter+15;
        while ((time_counter<timeout) && (c!=',') && (c!='\n') && (c!='\r')) {
            c=uart_getc_nowait();
            if ((c>='0' && c<='9' && i<5)) {
                svp[i]=c;
                //fprintf(UART, "read svp %c @ %d\n", c, i);
                i++;
            }
        }
        if (c==',' && time_counter<timeout) { // only go on when the instruction format is correct
            i=0;
            c=0;
            timeout=time_counter+15;
            while ((time_counter<timeout) && (c!=',') && (c!='\n') && (c!='\r')) {
                c=uart_getc_nowait();
                if ((c>='0' && c<='9' && i<5)) {
                    svm[i]=c;
                    //fprintf(UART, "read svm %c @ %d\n", c, i);
                    i++;
                }
            }
            if (i>0) ok=1;
        }

    }

    if (time_counter<timeout) { // process instruction if there was no timeout
        if (ok!=0) {
            signed int ok;
            ok=twi_24c02_writebyte(0, 0xAF);
            if (ok!=0) ok=twi_24c02_writebyte(1, 0xFE);
            if (ok!=0) ok=twi_24c02_writebytes(2, (uint8_t*)sn, 10);
            char* e;
            uint16_t v=strtoul(svp, &e, 10);
            if (ok!=0) ok=twi_24c02_writebytes(12, (uint8_t*)&v, 2);
            uint16_t v1=strtoul(svm, &e, 10);
            if (ok!=0) ok=twi_24c02_writebytes(14, (uint8_t*)&v1, 2);
            if (ok!=0) fprintf(UART, "%s, %d, %d\n\n", sn, v, v1);
            else uart_puts("ERROR: EEPROM WRITE ERROR");
        } else fprintf(UART, "ERROR: WRONG FORMAT name=%s svp=%s svm=%s\n\n", sn, svp, svm);
    } else uart_puts("ERROR: TIMEOUT\n\n");
    configSave();
}

// set the controller parameters P<NAME>=<WERT>
void command_setparameter(void) {
    char sn[20]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char sv[20]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i=0;
    int c=0;
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='=') && (c!='\n') && (c!='\r') && (i<19)) {
        c=uart_getc_nowait();
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
            c=uart_getc_nowait();
            if ((c>='0' && c<='9')
                ||((c>='A') && (c<='Z'))
                ||((c>='a') && (c<='z'))
                ||(c=='_')||(c==' ')||(c=='.')||(c==',')||(c==':')||(c==';')||(c=='/')||(c=='\\')||(c=='+')||(c=='*')||(c=='#')||(c=='-')) {

                sv[i]=c;
                i++;
            }
        }
    }
    if (time_counter<timeout) { // process instruction if there was no timeout
        if (strcmp("FAN_START", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') FAN_START=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("FAN_FACTOR", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') FAN_FACTOR=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("FAN_OFFSET", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') FAN_OFFSET=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("LCD_BACKLIGHT", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') LCD_BACKLIGHT=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("CURRENT_MAX", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') CURRENT_MAX=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("CURRENT_100", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') CURRENT_100=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("IADC_MEASUREMENTS", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') IADC_MEASUREMENTS=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("EADC_MEASUREMENTS", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') EADC_MEASUREMENTS=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("EADC_WAIT", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') EADC_WAIT=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("IADC_WAIT", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') IADC_WAIT=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("LED_BLINK", sn)==0) {
            char *e;
            uint16_t v=strtoul(sv, &e, 10);
            if (*e=='\0') LEDBLINK=v; else uart_puts("ERROR: INVALID DATA\n\n");
        } else if (strcmp("LED", sn)==0) {
            int i=0;
            char found=0;
            for (i=0; i<led_count; i++) {
                char n[20];
                strcpy_P(n, (PGM_P)pgm_read_word(&(led_names[i])));
                if (strcmp(n,sv)==0) {
                    LED=i;
                    found=1;
                    break;
                }
            }
            //fprintf(UART, "'%s' = %d\n\n", sv, LED);
            if (found==0) uart_puts("ERROR: UNKNOWN LED\n\n");
        } else if (strcmp("BAUDRATE", sn)==0) {
            char *e;
            uint32_t v=strtoul(sv, &e, 10);
            if (*e=='\0') {
                BAUDRATE=v;
                uart_init(BAUDRATE, 8, 1, UART_NOPARITY);
            } else uart_puts("ERROR: INVALID DATA\n\n");
        } else uart_puts("ERROR: UNKNOWN PARAMETER\n\n");
    } else uart_puts("ERROR: TIMEOUT\n\n");
    configSave();
}

// switch on and off output current (one digit 0 or 1)
void command_currentonoff(void) {
    char s[2]= {0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<1)) {
        c=uart_getc_nowait();
        if (c=='0'||
                c=='1') {
            s[i]=c;
            i++;
        }
    }
    if (time_counter<timeout) { // process instruction if there was no timeout
        if (s[0]=='0') current_on=0;
        else current_on=1;
    } else uart_puts("ERROR: TIMEOUT\n\n");
}


// switch on and off TTL output (one digit 0 or 1)
void command_outputonoff(void) {
    char s[2]= {0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+15;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<1)) {
        c=uart_getc_nowait();
        if (c=='0'||
                c=='1') {
            s[i]=c;
            i++;
        }
    }
    if (time_counter<timeout) { // process instruction if there was no timeout
        if (s[0]=='0') {
            ttl_off();
        } else {
            ttl_on();
        }
    } else uart_puts("ERROR: TIMEOUT\n\n");
}




int main(void) {
    uint8_t fananim_cmax;
    int16_t p=0;




    current_on=0;
    button_UPDOWN_length=0;
    button_MENU_state=1;
    lcd_load_default_glyphs_P=lcd_load_default_glyphs;
    cli();
    configLoad();
    InitHardware();
    PWM0_enable();
    PWM2_enable();
    twi_init();
    lcd_init();
    lcd_load_default_glyphs();
    uart_init(BAUDRATE, 8, 1, UART_NOPARITY);
    LCD=fdevopen(lcd_write, NULL);          //LCD-Ausgabe initialisieren
    UART=fdevopen(uart_putchar, uart_getchar);
    lcd_clear();
    lcd_home();

    fprintf(LCD, "\f-= PCCS  v%ld.%ld =-\n (c)%s.%s JK ", MAJOR, MINOR, MONTH, YEAR);
    _delay_ms(1000);
    command_info();
    button_ONOFF_state=button_ONOFF();


    timer1_enable();
    sei();


    while(1) {

        /*
          fan control:
          if temperature>FAN_START the fan is started:
          the PWM value for the fan is at least FAN_OFFSET. The regulation ist simply linear:

        	  PWM = FAN_OFFSET + (temperature-FAN_START)*FAN_FACTOR

          if PWM>255 the fan is driven at its maximum (255)

          NOTE: Fan regulation is also active when editor is enabled (we do not want to overheat anything!)
        */
        // read onboard temperatureersture sensor
        temperature=TEMP_read();
        if (temperature>FAN_START) {
            p=FAN_OFFSET+(temperature-FAN_START)*FAN_FACTOR;
            if (p>255) pwmfan=255;
            else pwmfan=p;
            PWM2_set(pwmfan);
        } else {
            pwmfan=0;
            PWM2_set(0);
        }

        // now we distinguish between editor and non-editor mode
        if (editor_enabled==1) {
            timer1_disable();
            editorStart(LCD);
            uart_init(BAUDRATE, 8, 1, UART_NOPARITY);
            _delay_ms(50);
            UART=fdevopen(uart_putchar, uart_getchar);
            uart_puts("---=== PCCS  v1.0 ===---\n");
            uart_puts(" (c) 2010 by Jan Krieger (DKFZ)\n");
            uart_puts(" j.krieger@dkfz.de\n");
            uart_puts(" jan@jkrieger.de\n\n");
            timer1_enable();
        } else {

            // NON-EDITOR/STANDARD MODE


            if (time_counter%DISPLAY_UPDATEANIMATION==0) {
                // display fan running as symbol
                fan_animation_counter++;
                fananim_cmax=4;
                if (pwmfan>=FAN_OFFSET) fananim_cmax=3;
                if (pwmfan>=FAN_OFFSET+(255-FAN_OFFSET)/3) fananim_cmax=2;
                if (pwmfan>=FAN_OFFSET+(255-FAN_OFFSET)/3*2) fananim_cmax=1;
                if (pwmfan==0) {
                    lcd_pos_string(6, 2, "[  ");
                } else if (fan_animation_counter>fananim_cmax) {
                    fan_animation_state++;
                    fan_animation_counter=0;
                    if (fan_animation_state>4) fan_animation_state=1;
                    switch(fan_animation_state) {
                    case 1:
                        lcd_pos_string(6, 2, "[\x05 ");
                        break;
                    case 2:
                        lcd_pos_string(6, 2, "[\x06 ");
                        break;
                    case 3:
                        lcd_pos_string(6, 2, "[\x07 ");
                        break;
                    case 4:
                        lcd_pos_string(6, 2, "[\x08 ");
                        break;
                    }
                }
            }

            // set LCD backligh intensity
            PWM0_set(LCD_BACKLIGHT);



            // switch output (and LED under ON/OFF button) on and off
            if ((current_on==0) || (CURRENT<=0)) {
                ledcurrent_off();
                timer0_disableTOIE();
            } else {
                if (LEDBLINK>0) {
                    timer0_enableTOIE();
                } else {
                    timer0_disableTOIE();
                    ledcurrent_on();
                }
            }
            if (current_on==0) {
                led_off();
            } else {
                led_on();
            }
            
            // measure current on ADC 5 each step is 4mA
            current_measured=ADC_read(5, IADC_MEASUREMENTS, IADC_WAIT)*4;

            // set curent via DAC
            if (CURRENT>CURRENT_MAX) CURRENT=CURRENT_MAX;
            DAC_set(CURRENT);

            char eeprom=' ';
            if (time_counter%LED_EEPROM_UPDATE==0) {
                if (bit_is_set(PINA, PA7)) { // check whether an EEPROM is connected (i.e. PA7 is pulled to 1
                    eeprom='x';
                    if ((twi_24c02_readbyte(0,NULL)==0xAF) && (twi_24c02_readbyte(1,NULL)==0xFE)) { // check first two bytes of EEPROM, which have to be 0xAF and 0xFE to be valid
                        twi_24c02_readbytes((uint8_t*)LED_NAME, 2, 10);
                        LED=0;
                        twi_24c02_readbytes((uint8_t*)&CURRENT_100, 12, 2);
                        twi_24c02_readbytes((uint8_t*)&CURRENT_MAX, 14, 2);
                        eeprom='e';
                    }

                }
            }

            if (time_counter%DISPLAY_UPDATE==0) {
                if (current_on==0) {
                    char str[25];
                    sprintf(str, "Iset=%dmA %c        ", CURRENT, eeprom);
                    lcd_pos_string(0,1,str);
                    sprintf(str, "%d°C]        ", temperature);
                    lcd_pos_string(9,2,str);
                } else {
                    char str[25];
                    if (LEDBLINK>0) {
                        sprintf(str, "IB=%d|%dmA %c        ", CURRENT, current_measured, eeprom);
                    } else {
                        sprintf(str, "IC=%d|%dmA %c        ", CURRENT, current_measured, eeprom);
                    }
                    lcd_pos_string(0,1,str);
                    sprintf(str, "%d°C]        ", temperature);
                    lcd_pos_string(9,2,str);

                }
                if (CURRENT_100>0) {
                    char str[20];
                    uint16_t p=((uint32_t)CURRENT*100)/(uint32_t)CURRENT_100;
                    sprintf(str, "%4d%% ", p);
                    lcd_pos_string(0,2,str);
                }

                // plot button text:
                lcd_pos_string(14, 1, "\x01\x03");
                lcd_pos_string(14, 2, "\x02\x04");

            }
        }




        int c=uart_getc_nowait();
        gc=c;
        switch (c) {
        case '?':
            command_info();
            break;
        case 'I':
            command_setcurrent();
            break;
        case 'L':
            command_currentonoff();
            break;
        case 'O':
            command_outputonoff();
            break;
        case 'P':
            command_setparameter();
            break;
        case 'S':
            command_state();
            break;
        case 'A':
            command_analogin();
            break;
        case 'M':
            command_mspulse();
            break;
        case 'U':
            command_uspulse();
            break;
        case 'V':
            command_version();
            break;
        case 'T':
            command_get_temperature();
            break;
        case 'G':
            command_get_setcurrent();
            break;
        case 'C':
            command_get_measuredcurrent();
            break;
        case 'N':
            command_get_ledonoff();
            break;
        case 'W':
            command_write_eeprom();
            break;
        case 'D':
            command_dump_eeprom();
            break;
        case 'Y':
            command_get_led();
            break;
        case 'X':
            command_get_max_current();
            break;
        case '%':
            command_get_100perc_current();
            break;
        case 'q':
            command_get_manufacturer();
            break;
        case 'Q':
            command_set_manufacturer();
            break;
        case 'j':
            command_get_wavelength();
            break;
        case 'J':
            command_set_wavelength();
            break;
        case 'f':
            command_get_power();
            break;
        case 'F':
            command_set_power();
            break;
        default:
        case -1:
            break;
        }
    }

    return 0;
}
