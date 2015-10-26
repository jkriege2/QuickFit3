
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "tools.h"
#include "uart.h"
#include "version.h"


#define SERVO_INTERRUPTS 20000
#define SERVO_INTERRUPTLENGTH_MICROSECONDS (1000000/SERVO_INTERRUPTS)
#define SERVO_PULSEWIDTH_MICROSECONDS 18000
#define SERVO_STATEMAX (2100/SERVO_INTERRUPTLENGTH_MICROSECONDS)
#define SERVO_STATEMIN (950/SERVO_INTERRUPTLENGTH_MICROSECONDS)

#define SERVO_STATE1 (1050/SERVO_INTERRUPTLENGTH_MICROSECONDS)
#define SERVO_STATE2 (1300/SERVO_INTERRUPTLENGTH_MICROSECONDS)


// variable declaration
FILE* UART;

#define NUM_SERVOS 4
#define BAUDRATE 9600
#define SERIAL_TIMEOUT 5
volatile uint32_t time_counter=0; // wall clock with +15 per second
volatile uint16_t micro_count=0;
volatile char gc;
volatile uint8_t servo_set[NUM_SERVOS];
volatile uint16_t servo_count=0;

volatile uint8_t button_state=0;
volatile uint8_t button_commands=0;

/*

    SERVO CONTROL

    1. PWM-Signal:
    ~~~~~~~~~~~~~~

     <---------- 20ms --------->
     +---+                      +---+
     |   |                      |   |
    -+   +----------------------+   +-
     <--->
       950-2050탎

    PWM1/PWM0 = 950/20000 ... 2050/20000 = 4.75% ... 10.25%
    PWM1_max-PWM1_min = 1100탎

    => for 10탎 timer intervals ~110 steps from min to max
        => @ 16MHz: 160 instructions
        => 100,000 interrupts per second

    => for 100탎 timer intervals ~11 steps from min to max
        => @ 16MHz: 1600 instructions
        => 10,000 interrupts per second

    => 11 steps is enough for servo control for shutters and then there
       are enough resources left for RS232 interface etc.

 */



// interrupt service routine for timer/counter2 compare A interrupt => 10000 calls/second
SIGNAL(TIMER2_COMPA_vect) {
    cli();
    micro_count++;
    if (micro_count>SERVO_INTERRUPTS/10) {
        time_counter++; // increase time_counter by +10 per second
        micro_count=0;
    }

    if (servo_count>servo_set[0]) {
        servo1_pwm_off();
    } else {
        servo1_pwm_on();
    }

    if (servo_count>servo_set[1]) {
        servo2_pwm_off();
    } else {
        servo2_pwm_on();
    }

    if (servo_count>servo_set[2]) {
        servo3_pwm_off();
    } else {
        servo3_pwm_on();
    }

    if (servo_count>servo_set[3]) {
        servo4_pwm_off();
    } else {
        servo4_pwm_on();
    }

    if (servo_count<(SERVO_PULSEWIDTH_MICROSECONDS/SERVO_INTERRUPTLENGTH_MICROSECONDS-1)) servo_count++;
    else servo_count=0;

    sei();
}

// sends status information to UART
void command_info(void) {
    uart_puts("Servo Controller\n");
    fprintf(UART, " (c) %s.%s by Jan Krieger (DKFZ)\n", MONTH, YEAR);
    uart_puts(" j.krieger@dkfz.de --  jan@jkrieger.de\n\n");
}

// sends status information to UART
void command_version(void) {
    fprintf(UART, "%s (%s.%s)\n\n", FULLVERSION_STRING, MONTH, YEAR);
}


void command_setservo(void) {
    char s[4]= {0,0,0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+SERIAL_TIMEOUT;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<2)) {
        c=uart_getc_nowait();
        if (c>='0' && c<='9') {
            s[i]=c;
            i++;
        }

    }

    if (time_counter<timeout) { // process instruction if there was no timeout
        switch (s[0]) {
            case '1':
                servo_set[0]=((s[1]=='1')?SERVO_STATE2:SERVO_STATE1);
                break;
            case '2':
                servo_set[1]=((s[1]=='1')?SERVO_STATE2:SERVO_STATE1);
                break;
            case '3':
                servo_set[2]=((s[1]=='1')?SERVO_STATE2:SERVO_STATE1);
                break;
            case '4':
                servo_set[3]=((s[1]=='1')?SERVO_STATE2:SERVO_STATE1);
                break;
            default:
                uart_puts("ERROR: INVALID SERVO\n\n");
                break;
        }

    } else uart_puts("ERROR: TIMEOUT\n\n");
}


// read a servo state
void command_getservo(void) {
    char s[4]= {0,0,0,0};
    int i=0;
    int c='X';
    uint32_t timeout=time_counter+SERIAL_TIMEOUT;
    while ((time_counter<timeout) && (c!='\n') && (c!='\r') && (i<1)) {
        c=uart_getc_nowait();
        if (c>='0' && c<='9') {
            s[i]=c;
            i++;
        }

    }

    if (time_counter<timeout) { // process instruction if there was no timeout
        switch (s[0]) {
            case '1':
                if (servo_set[0]==SERVO_STATE2) uart_puts("1\n\n");
                else uart_puts("0\n\n");
                break;
            case '2':
                if (servo_set[1]==SERVO_STATE2) uart_puts("1\n\n");
                else uart_puts("0\n\n");
                break;
            case '3':
                if (servo_set[2]==SERVO_STATE2) uart_puts("1\n\n");
                else uart_puts("0\n\n");
                break;
            case '4':
                if (servo_set[3]==SERVO_STATE2) uart_puts("1\n\n");
                else uart_puts("0\n\n");
                break;
            default:
                uart_puts("ERROR: INVALID SERVO\n\n");
                break;
        }

    } else uart_puts("ERROR: TIMEOUT\n\n");
}



int main(void) {
    cli();
    InitHardware();
    int i;
    for (i=0; i<NUM_SERVOS; i++) {
        servo_set[i]=SERVO_STATE1;
    }
    button_state=0;
    time_counter=0;

    uart_init(BAUDRATE, 8, 1, UART_NOPARITY);
    UART=fdevopen(uart_putchar, uart_getchar);
    command_info();
    //timer1A_enable(10);
    timer2A_enable(SERVO_INTERRUPTS);
    sei();
    uint32_t old_time=time_counter;
    uint8_t old_button1=servo1_button();
    uint8_t old_button2=servo2_button();
    uint8_t old_button3=servo3_button();
    uint8_t old_button4=servo4_button();
    while(1) {

        if (old_time!=time_counter) {
            old_time=time_counter;
            if (servo1_button() && (old_button1!=servo1_button())) {
                if (servo_set[0]==SERVO_STATE2) {
                    servo_set[0]=SERVO_STATE1;
                } else {
                    servo_set[0]=SERVO_STATE2;
                }
            }

            if (servo2_button() && (old_button2!=servo2_button())) {
                if (servo_set[1]==SERVO_STATE2) {
                    servo_set[1]=SERVO_STATE1;
                } else {
                    servo_set[1]=SERVO_STATE2;
                }
            }

            if (servo3_button() && (old_button3!=servo3_button())) {
                if (servo_set[2]==SERVO_STATE2) {
                    servo_set[2]=SERVO_STATE1;
                } else {
                    servo_set[2]=SERVO_STATE2;
                }
            }

            if (servo4_button() && (old_button4!=servo4_button())) {
                if (servo_set[3]==SERVO_STATE2) {
                    servo_set[3]=SERVO_STATE1;
                } else {
                    servo_set[3]=SERVO_STATE2;
                }
            }


            old_button1=servo1_button();
            old_button2=servo2_button();
            old_button3=servo3_button();
            old_button4=servo4_button();
        }



        // update LEDs
        if (servo_set[0]==SERVO_STATE2) {
            servo1_led_on();
        } else {
            servo1_led_off();
        }
        if (servo_set[1]==SERVO_STATE2) {
            servo2_led_on();
        } else {
            servo2_led_off();
        }
        if (servo_set[2]==SERVO_STATE2) {
            servo3_led_on();
        } else {
            servo3_led_off();
        }
        if (servo_set[3]==SERVO_STATE2) {
            servo4_led_on();
        } else {
            servo4_led_off();
        }

        // read commands from serial port
        int c=uart_getc_nowait();
        gc=c;
        switch (c) {
        case 'S':
        case 's':
            command_setservo();
            break;
        case 'Q':
        case 'q':
            command_getservo();
            break;
        case '?':
            command_info();
            break;
        case 'V':
        case 'v':
            command_version();
            break;
        default:
        case -1:
            break;
        }
    }

    return 0;
}

