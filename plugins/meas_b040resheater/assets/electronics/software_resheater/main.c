/*
  this file:     Main functions
  project:       TEC Controller
  author:        Jan Soldat
                 jan.soldat@gmail.com

  target system:     Atmel ATmega32, no external RAM
                     XTAL: 16 MHz
  compiler (tested): avr-gcc 4.3.3

For changes in temperature sensor amplifier board or TEC driver board:

- Pt100 temperature sensor readout is done in read_adc() in tools.c, conversion to 1/10°C in main loop
- Setting maximum TEC voltage and SET_TEMP for Max1968 TEC driver is done in main function, using the set_dac_41100() function from tools.c
- Setting output SET_TEMP (Vctli voltage for Max1968) is done in main loop, once for SET_TEMP control and once for temperature control, using set_dac_4921() function from tools.c

*/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "lcd.h"		//Library functions for HD44780U LCD by Peter Fleury
#include "tools.h"
#include "uart.h"
#include "config.h"
#include "pid.h"
#include "screen.h"
#include "editor.h"
#include "version.h"
#include <string.h>


// define set temperature range in 1/10 °C
#define SET_TEMPERATURE_MIN 0
#define SET_TEMPERATURE_MAX 1500

// overflows of T/C1A (i.e. calls to SIGNAL(TIMER1_COMPA_vect) ) per second
// this is the central chonometer!
// 1s*cntA_MAX/TIMER1A_OVERFLOWS is the fequency of the fan PWM!
#define TIMER1A_OVERFLOWS 200

// TIMER1A_OVERFLOWS times per second pid_counter is increased by 1. If pid_counter reached PID_COUNTER_WHEN
// the PID control is recalculated 1s/(200/10) is the time between two updates of regulator output
#define PID_COUNTER_WHEN 200

// every cntA_MAX ticks, the buttons are read
#define cntA_MAX 10




char error=0;
char sensor_status0=1;
char sensor_status1=1;
volatile uint32_t time_counter=0;
volatile int16_t int_temp=0;
volatile int32_t temp0=0;
volatile int32_t temp1=0;
volatile uint8_t sensor_status;	//0:temp0&1 connected, 1: temp0 disconnected, 2: temp1 disconnected, 3: both disconnected
int16_t setvalue=0;

volatile uint8_t heating_state=0;
volatile uint8_t fan_speed=0;


volatile uint8_t counter=0;
volatile uint16_t pid_counter=0;

// execute a PID update only every <this> main loop cycles
#define PIDUPDATE_VALUE 50
// execute a PID update only every <this> main loop cycles
#define SET_TEMPUPDATE_VALUE 10


//volatile int32_t err_0 = 0;  // SET_TEMP error (t =   0)
//volatile int32_t err_1 = 0;  // prev.   error (t = - T)
volatile PID_PARAMS pid;


FILE* LCD;
FILE* UART;

//lookup table for kty81/220 temperature sensor (in program space)
//created by fitting a x^2 polynomial to the values given in datasheet
//and calculating temperature for each ADC value (see spreadsheet)
volatile int16_t kty81220[] PROGMEM={-44,-44,-43,-43,-42,-41,-41,-40,-40,-39,-39,-38,-37,-37,-36,-36,-35,-34,-34,-33,-33,-32,-32,-31,-30,-30,-29,-29,-28,-28,-27,-26,-26,-25,-25,-24,-23,-23,-22,-22,-21,-21,-20,-19,-19,-18,-18,-17,-16,-16,-15,-15,-14,-13,-13,-12,-12,-11,-10,-10,-9,-9,-8,-8,-7,-6,-6,-5,-5,-4,-3,-3,-2,-2,-1,0,0,1,1,2,3,3,4,4,5,6,6,7,7,8,9,9,10,10,11,12,12,13,14,14,15,15,16,17,17,18,18,19,20,20,21,21,22,23,23,24,25,25,26,26,27,28,28,29,29,30,31,31,32,33,33,34,34,35,36,36,37,38,38,39,39,40,41,41,42,43,43,44,44,45,46,46,47,48,48,49,49,50,51,51,52,53,53,54,55,55,56,56,57,58,58,59,60,60,61,62,62,63,63,64,65,65,66,67,67,68,69,69,70,71,71,72,72,73,74,74,75,76,76,77,78,78,79,80,80,81,82,82,83,83,84,85,85,86,87,87,88,89,89,90,91,91,92,93,93,94,95,95,96,97,97,98,99,99,100,101,101,102,103,103,104,105,105,106,106,107,108,108,109,110,110,111,112,112,113,114,115,115,116,117,117,118,119,119,120,121,121,122,123,123,124,125,125,126,127,127,128,129,129,130,131,131,132,133,133,134,135,135,136,137,137,138,139,140,
140,141,142,142,143,144,144,145,146,146,147,148,148,149,150,151};




volatile uint8_t button_ONOFF_state; // stores the state of ON/OFF in the last ISR call
volatile uint8_t button_MENU_state; // stores the state of MENU in the last ISR call
volatile uint8_t button_UPDOWN_length; // how long are the buttons already pressed
//volatile uint32_t time_counter=0; // wall clock with +15 per second

volatile uint8_t cntA=0;
volatile uint8_t cntB=0;


// interrupt service routine for timer/counter1 compare A interrupt
SIGNAL(TIMER1_COMPA_vect) {
    pid_counter++;
    // software PWM (10 steps) for internal fan
    if (fan_speed>0) {
        if (cntA==0) {
            intfan_on();
        } else if (cntA>fan_speed) {
            intfan_off();
        }
    } else {
        intfan_off();
    }
    cntA++;

    if (cntA>=cntA_MAX) {
        cntA=0;

        time_counter++;
        // auxiliary variables




        /*
          if parameter editor is active we do not change the LED settings,
          but we only call the editor's interrupt routine!
        */
        if (editor_enabled == 0) {
            uint8_t a;
            uint8_t b;
            // check menu button
            a=button_MENU();
            if ((a!=0) && (button_MENU_state==0)) {
                editor_enabled=1;
            }
            button_MENU_state=a; // store button state


            if (editor_enabled == 0) {

                // switch output SET_TEMP on/off, depending on the variable SET_TEMP_on
                // and update SET_TEMP_on depending on button click
                b=button_ONOFF();
                if ((b!=0) && (button_ONOFF_state==0)) {
                    if (heating_state==0) {
                        heating_state=1;
                        //btnled_on();
                    } else {
                        heating_state=0;
                        //btnled_off();
                    }
                }
                button_ONOFF_state=b; // store button state



                a=button_UP();
                b=button_DOWN();
                if (b==0) {
                    if (button_UPDOWN_length>40) {
                        if (SET_TEMP>=SET_TEMPERATURE_MIN +10) {
                            SET_TEMP=SET_TEMP-10;
                            configSaveNonMenu();
                        } else {
                            SET_TEMP=SET_TEMPERATURE_MIN;
                            configSaveNonMenu();
                        }
                    } else if (button_UPDOWN_length>20) {
                        if (SET_TEMP>=SET_TEMPERATURE_MIN+10) {
                            SET_TEMP=SET_TEMP-10;
                            configSaveNonMenu();
                        } else {
                            SET_TEMP=SET_TEMPERATURE_MIN;
                            configSaveNonMenu();
                        }
                    } else {
                        button_UPDOWN_length++;
                        if ((SET_TEMP>=SET_TEMPERATURE_MIN+1) && (button_UPDOWN_length%2==0)) {
                            SET_TEMP=SET_TEMP-1;
                            configSaveNonMenu();
                        }
                    }
                } else if (a==0) {
                    if (button_UPDOWN_length>40) {
                        if (SET_TEMP<=SET_TEMPERATURE_MAX-20) {
                            SET_TEMP=SET_TEMP+20;
                            configSaveNonMenu();
                        } else {
                            SET_TEMP=SET_TEMPERATURE_MAX;
                            configSaveNonMenu();
                        }
                    } else if (button_UPDOWN_length>20) {
                        if (SET_TEMP<=SET_TEMPERATURE_MAX-10) {
                            SET_TEMP=SET_TEMP+10;
                            configSaveNonMenu();
                        } else {
                            SET_TEMP=SET_TEMPERATURE_MAX;
                            configSaveNonMenu();
                        }
                    } else {
                        button_UPDOWN_length++;
                        if (SET_TEMP<=SET_TEMPERATURE_MAX-1 && (button_UPDOWN_length%2==0)) {
                            SET_TEMP=SET_TEMP+1;
                            configSaveNonMenu();
                        }
                    }
                } else {
                    button_UPDOWN_length=0;
                }

            }




        } else {
            editorButtonInt();
        }
    }

}





//sets the internal fan speed according to temperature threshold defines
//using Timer1 Output Compare A as 16 bit resolution PWM
//activates LED 1 if there's a thermal problem
uint16_t fan_control(int16_t int_temp, int16_t SHUTDOWN, int16_t FAN_MIN, uint16_t FAN_START, uint16_t FAN_MAX, int16_t FAN_INCREASE) {
	if (int_temp > SHUTDOWN)		//temperature threshold reached, turn off output
    {
		return FAN_MAX;
	}
	else if (int_temp < FAN_MIN)	//temperature low, no need for fan
	{
		return 0;
	}
	else
	{
//set the fan speed between minimum speed and almost full speed linear with internal temperature
		int16_t r=FAN_START+(int_temp-FAN_MIN)*FAN_INCREASE/10;
		if (r<FAN_START) return FAN_START;
		else if (r>FAN_MAX) return FAN_MAX;
		else return r;
	}
}








// sends status information to UART
void command_info(void) {
    fprintf(UART, "\n---=== Resitance Heater Temperature Control  v%s ===---\n", FULLVERSION_STRING);
    fprintf(UART, " (c) %s.%s by Jan Krieger (DKFZ)\n", MONTH, YEAR);
    uart_puts(" j.krieger@dkfz.de --  jan@jkrieger.de\n\n");
}

// sends status information to UART
void command_version(void) {
    fprintf(UART, "%s (%s.%s)\n\n", FULLVERSION_STRING, MONTH, YEAR);
}

// sends comma separated values of the
//   transmits: temp0, temp1, SET_TEMP, setvalue, heating_state, error, int_temp
void command_state(void) {
    fprintf(UART, "%ld, %ld, %d, %d, %d, %d, %d\n\n", temp0, temp1, SET_TEMP, setvalue, (heating_state==0)?0:1, error, int_temp);
}

#define PARAM_TEMP0 0
#define PARAM_TEMP1 1
#define PARAM_SETTEMP 2
#define PARAM_SETVALUE 3
#define PARAM_HEATINGSTATE 4
#define PARAM_ERROR 5
#define PARAM_INTTEMP 6
void command_getparam(int param) {
    switch (param) {
        case PARAM_TEMP0: fprintf(UART, "%ld\n\n", temp0); break;
        case PARAM_TEMP1: fprintf(UART, "%ld\n\n", temp1); break;
        case PARAM_SETTEMP: fprintf(UART, "%d\n\n", SET_TEMP); break;
        case PARAM_SETVALUE: fprintf(UART, "%d\n\n", setvalue); break;
        case PARAM_HEATINGSTATE: fprintf(UART, "%d\n\n", (heating_state==0)?0:1); break;
        case PARAM_ERROR: fprintf(UART, "%d\n\n", error); break;
        case PARAM_INTTEMP: fprintf(UART, "%d\n\n", int_temp); break;
    }
}

// read a current (<4 digits) from UART and set the current to this value
void command_settemp(void) {
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
        int16_t cu=strtol(s, &e, 10);
        if (*e=='\0') {
            SET_TEMP=cu;
            configSaveNonMenu();
        } else uart_puts("ERROR: INVALID DATA\n\n");
    } else uart_puts("ERROR: TIMEOUT\n\n");

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

        if (time_counter<timeout) { // process instruction if there was no timeout
            if (strcmp("T_INPUT", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if ((*e=='\0')&&(v<3)) T_INPUT=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("PT100_OFFSET0", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if (*e=='\0') PT100_OFFSET0=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("PT100_OFFSET1", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if (*e=='\0') PT100_OFFSET1=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("SHUTDOWN", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if (*e=='\0') SHUTDOWN=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("FAN_MIN", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if (*e=='\0') FAN_MIN=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("FAN_INCREASE", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if (*e=='\0') FAN_INCREASE=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("PID_I", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if (*e=='\0') PID_I=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("PID_IMAX", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if (*e=='\0') PID_IMAX=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("PID_P", sn)==0) {
                char *e;
                int16_t v=strtol(sv, &e, 10);
                if (*e=='\0') PID_P=v; else uart_puts("ERROR: INVALID DATA\n\n");
            } else if (strcmp("BAUDRATE", sn)==0) {
                char *e;
                uint32_t v=strtoul(sv, &e, 10);
                if (*e=='\0') {
                    BAUDRATE=v;
                    uart_init(BAUDRATE, 8, 1, UART_NOPARITY);
                } else uart_puts("ERROR: INVALID DATA\n\n");
            } else uart_puts("ERROR: UNKNOWN PARAMETER\n\n");
        } else uart_puts("ERROR: TIMEOUT\n\n");
    } else if (c=='\n' || c=='\r') {


        if (time_counter<timeout) { // process instruction if there was no timeout
            if (strcmp("T_INPUT", sn)==0) {
                fprintf(UART, "%d\n\n", T_INPUT);
            } else if (strcmp("PT100_OFFSET0", sn)==0) {
                fprintf(UART, "%d\n\n", PT100_OFFSET0);
            } else if (strcmp("PT100_OFFSET1", sn)==0) {
                fprintf(UART, "%d\n\n", PT100_OFFSET1);
            } else if (strcmp("SHUTDOWN", sn)==0) {
                fprintf(UART, "%d\n\n", SHUTDOWN);
            } else if (strcmp("FAN_MIN", sn)==0) {
                fprintf(UART, "%d\n\n", FAN_MIN);
            } else if (strcmp("FAN_INCREASE", sn)==0) {
                fprintf(UART, "%d\n\n", FAN_INCREASE);
            } else if (strcmp("PID_I", sn)==0) {
                fprintf(UART, "%d\n\n", PID_I);
            } else if (strcmp("PID_IMAX", sn)==0) {
                fprintf(UART, "%ld\n\n", PID_IMAX);
            } else if (strcmp("PID_P", sn)==0) {
                fprintf(UART, "%d\n\n", PID_P);
            } else if (strcmp("BAUDRATE", sn)==0) {
                fprintf(UART, "%ld\n\n", BAUDRATE);
            } else uart_puts("ERROR: UNKNOWN PARAMETER\n\n");

        } else uart_puts("ERROR: TIMEOUT\n\n");
    }
    configSave();
}



// switch on and off the output (one digit 0 or 1)
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
        if (s[0]=='0') heating_state=0;
        else heating_state=1;
    } else uart_puts("ERROR: TIMEOUT\n\n");
}







int main(void) {

    avr_init();			//Init Hardware
    led_2_off();
    //configIOPins();

	button_UPDOWN_length=0;
	button_ONOFF_state=1;//button_ONOFF();
	button_MENU_state=1;//button_MENU();


	editor_enabled=0;



    lcd_init(0,0);
    lcd_clear();
    lcd_home();
//	setup a file stream for the LCD display
	LCD = fdevopen(lcd_write, NULL);

	configLoad(); // load configuration from EEPROM
	uart_init(BAUDRATE, 8, 1, UART_NOPARITY);
	UART=fdevopen(uart_putchar, uart_getchar);
    pid_set((PID_PARAMS*)&pid, PID_P, PID_D, PID_I, PID_IMAX);//PID_P, PID_I);
	/*if (LCD_BACKLIGHT) {LCDback_on();}	//init LCD backlight
	else {LCDback_off();}*/

	//Splash screen
	lcd_string_line1("Resistance-Heater   ");
	lcd_string_line2("  (c)2011 J. Krieger");
	lcd_string_line3("     B040, DKFZ     ");
	lcd_string_line4("*+*+*+*+*+*+*+*+*+*+");
	command_info();
	_delay_ms(500);	//Pause for the splash screen



    sei();
    timer1A_enable(TIMER1A_OVERFLOWS);

	lcd_basic_screen(LCD);
    led_2_on();
	editor_enabled=0;

	PWM2_enable(5); // fPWM = 245Hz  = 16e6/510/128

    for(;;) {

        // measure internal temperature
        int_temp=read_inttemp(8);			//read internal temperature sensor
        int_temp -=191;						//offset: lookup table starts at ADC value of 191
        if ((int_temp > 0) && (int_temp < 307))	//check if adc value - offset is in the range of the lookup table
        {
            int_temp = pgm_read_dword(&kty81220[int_temp]); //look up the temperature in the table
        }

        // control internal fan and switch off, when needed!
        fan_speed=fan_control(int_temp, SHUTDOWN, FAN_MIN, cntA_MAX/2, cntA_MAX, (FAN_INCREASE*(int16_t)cntA_MAX)/10);
        if (int_temp>SHUTDOWN) {
            heating_state=0;
            led_1_on();
            error=1; // overtemperature
        } else {
            led_1_off();
            error=0;
        }

        // read temperature from PT100 interfaces
        temp0=PT1002T(read_adc(1,16), PT100_OFFSET0);
        temp1=PT1002T(read_adc(0,16), PT100_OFFSET1);
        sensor_status0=1;
        sensor_status1=1;
        if ((temp0==0xFFFF)||(temp0>9900)||(temp0<-9900)) sensor_status0=0;
        if ((temp1==0xFFFF)||(temp1>9900)||(temp1<-9900)) sensor_status1=0;

        if (heating_state!=0) {
            led_3_yellow();
            int32_t temp=temp0;
            char ok=sensor_status0;
            if (T_INPUT==1) { temp=temp1; ok=sensor_status1; }
            else if (T_INPUT==2) { temp=(temp0+temp1)/2; ok=((sensor_status0!=0)&&(sensor_status1!=0))?1:0; }

            if (ok!=0) {
                error=0;
                if (pid_counter>PID_COUNTER_WHEN) {
                    pid_counter=0;
                    setvalue=pid_control((PID_PARAMS*)&pid, SET_TEMP, temp);
                    if (setvalue<0) setvalue=0;
                    if (setvalue>255) setvalue=255;
                }

            } else {
                //heating_state=0;
                error=2;
                setvalue=0;
            }

        } else {
            led_3_off();
            setvalue=0;
        }
        PWM2_set(setvalue);


        if (editor_enabled==1) {
            led_2_off();
            led_1_off();
            led_3_off();
            editorStart(LCD);
            uart_init(BAUDRATE, 8, 1, UART_NOPARITY);
            _delay_ms(50);
            sei();
            lcd_basic_screen(LCD);
            led_2_on();
            editor_enabled=0;
            button_UPDOWN_length=0;
            button_ONOFF_state=1;
            button_MENU_state=1;
            pid_set((PID_PARAMS*)&pid, PID_P, PID_D, PID_I, PID_IMAX);
            //fprintf(LCD, "\f>%ld %d %d %d\n%d", time_counter, editor_enabled, sequence_command, sequence_onoff, SREG);
        } else {



            int c=uart_getc_nowait();
            switch (c) {
                case '?':
                    command_info();
                    break;
                case 'T':
                    command_settemp();
                    break;
                case 'O':
                    command_outputonoff();
                    break;
                case 'P':
                    command_setparameter();
                    break;
                case 's':
                    command_state();
                    break;
                case 'V':
                    command_version();
                    break;
                case '1':
                    command_getparam(PARAM_TEMP0);
                    break;
                case '2':
                    command_getparam(PARAM_TEMP1);
                    break;
                case 'i':
                    command_getparam(PARAM_INTTEMP);
                    break;
                case 'o':
                    command_getparam(PARAM_HEATINGSTATE);
                    break;
                case 'v':
                    command_getparam(PARAM_SETVALUE);
                    break;
                case 't':
                    command_getparam(PARAM_SETTEMP);
                    break;
                case 'e':
                    command_getparam(PARAM_ERROR);
                    break;

                default:
                case -1:
                    break;
            }




            lcd_display_temp(LCD, T_INPUT, temp0, temp1, int_temp, SET_TEMP);
            //lcd_display_PWM(LCD, SET_TEMP, SET_TEMPERATURE_MIN, SET_TEMPERATURE_MAX);
            lcd_display_PWM(LCD, setvalue, 0, 255);
            if (error>0) led_1_on();
            if (error==1) {
                lcd_pos_string(12,2,"overtemp")
            } else if (error==2) {
                lcd_pos_string(12,2," senserr")
            } else {
                lcd_pos_string(12,2,"        ")
            }

            if (heating_state!=0) {
                lcd_pos_string(15,4," Off ")
            } else {
                lcd_pos_string(15,4," On  ")
            }

        }



    }
    return(0);
}

