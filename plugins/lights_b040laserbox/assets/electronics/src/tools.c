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


#include "tools.h"



/*
  InitHardware() initializes the MC
*/
void InitHardware(void) {

    //set data direction: 1: output, 0: input
    DDRA=0b00010000;
    DDRB=0b10101001;
    DDRC=0b00011101;
    DDRD= 0b11111010;
    PORTD=0b00000000;

    /*
      activate SPI (Serial Peripheral Interface see datasheet pp. 130)

      SPCR:
        SPIE=0;          no Interrupt
        SPE=1;           SPI enabled
        DORD=0;          dataorder: MSB-first
        MSTR=1:          Master-Mode
        CPOL=0:          non-inverted clock
        CPHA=0:          sample on leading edge
        SPR0=0, SPR1=1:  f_osc/32 (together with SPI2X=1)
                       -> see also SPI2X in SPSR

      SPSR:
        SPI2X=1:         double-speed mode (see also SPR0/1)
    */
    SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR1);
    SPSR=(1<<SPI2X);




}




void PWM2A_enable(void) {
    /*
      timer 0 is used for PWM output for the LCD backlight

    - prescaler: divide by 1
    - fast PWM mode


    WGM21=1
    WGM20=1
    COM21=1
    COM20=0
    CS22=0
    CS21=0
    CS20=1
     */
    TCCR2A=(1<<WGM21)|(1<<WGM20)|(1<<COM2A1);
    TCCR2B=(1<<CS20);
    TCNT2=0;
    OCR2A=0;
}

void PWM2A_disable(void) {
    TCCR2A=0;
}

void PWM2A_set(uint8_t value) {
    OCR2A=value;
}


void timer1A_enable(uint16_t interrupts_per_sec) {
    unsigned char sreg;
    sreg = SREG;
    cli();

    /*
     initialize Timer1 for interrupt driven reading of push-button

     th timer runs at 7.3728MHz/1024 = 7.2 kHz
     ==> count from 0 to 1563. On counter=1563 ther will be a compare match,
        this makes about 10 overflow interrupts per second (use OCR1A)
     ==> count from 0 to 781. On counter=781 ther will be a compare match,
        this makes about 20 overflow interrupts per second (use OCR1A)

     TCCR1A = 0    keine PWM oder Ausgangs-Modi
     TCCR1B
       ICNC1=0
       ICES1=0
       Bit5=0
       WGM13=0
       WGM12=0
       CS12=1 |
       CS11=0 | Clk/1024 (Vorteiler)
       CS10=1 |

     TIMSK:
       TOIE1=1/0 => aktiviert/deaktiviert den T/C1-Overflow Interrupr
    */

    TIMSK1=0; //all timer interrupts disabled, T/C interrupt mask register f

    TCNT1=0;

    TCCR1A=0;
    TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10);


    TCNT1=0;
    //OCR1A=1563;
    OCR1A=(F_CPU/1024)/interrupts_per_sec;
    sbi(TIMSK1, OCIE1A);
    SREG = sreg;
}

void timer1B_enable(uint16_t interrupts_per_sec) {
    unsigned char sreg;
    sreg = SREG;
    cli();

    /*
     initialize Timer1 for interrupt driven reading of push-button

     th timer runs at 16MHz/1024 = 15,625 kHz
     ==> count from 0 to 1563. On counter=1563 ther will be a compare match,
        this makes about 10 overflow interrupts per second (use OCR1A)
     ==> count from 0 to 781. On counter=781 ther will be a compare match,
        this makes about 20 overflow interrupts per second (use OCR1A)

     TCCR1A = 0    keine PWM oder Ausgangs-Modi
     TCCR1B
       ICNC1=0
       ICES1=0
       Bit5=0
       WGM13=0
       WGM12=0
       CS12=1 |
       CS11=0 | Clk/1024 (Vorteiler)
       CS10=1 |

     TIMSK:
       TOIE1=1/0 => aktiviert/deaktiviert den T/C1-Overflow Interrupr
    */

    TIMSK1=0; //all timer interrupts disabled, T/C interrupt mask register f

    TCNT1=0;

    TCCR1A=0;
    TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10);


    TCNT1=0;
    //OCR1B=1563;
    OCR1B=(F_CPU/1024)/interrupts_per_sec;
    sbi(TIMSK1, OCIE1B);
    SREG = sreg;
}


uint16_t ADC_read(uint8_t channel, uint8_t measurements, uint16_t wait_us) {
    uint16_t result=0;
    uint8_t i;

    // select given channel
    uint8_t r=channel & 0x0F;
    ADMUX=r|0b11000000; // internal 2.56V reference

    // activate ADC and set clock prescaler to 4, i.e. <~2MHz
    ADCSRA = (1<<ADEN) | (1<<ADPS1);

    // after activation: do one dummy readout:
    ADCSRA |= (1<<ADSC);
    while ( ADCSRA & (1<<ADSC) ) {
        ;     // wait for end of conversion
    }
    result = ADCW;

    // now measure measurements times and average!
    result = 0;
    for( i=0; i<measurements; i++ ) {
        ADCSRA |= (1<<ADSC);            // one conversion
        while ( ADCSRA & (1<<ADSC) ) {
            ;   // wait for end of conversion
        }
        result += ADCW;		    // add up conversions
        if (wait_us>0) _delay_us(wait_us);
    }
    ADCSRA &= ~(1<<ADEN);             // deactivate ADC

    result /= measurements;                     // calculate average

    return result;

}


// write <data> to SPI
void spi_write(unsigned char data) {
    SPDR=data;                     // send data
    while(!(SPSR & (1<<SPIF)));  // wait until transmitter is ready
}

// write <data> to SPI and return whatever has been received
uint8_t spi_readwrite(unsigned char data) {
    SPDR=data;                     // send data
    while(!(SPSR & (1<<SPIF)));  // wait until transmitter is ready
    return SPDR;
}



