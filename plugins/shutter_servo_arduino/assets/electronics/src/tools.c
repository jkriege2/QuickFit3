#include "tools.h"
#include <string.h>


/*
  InitHardware() initializes the MC
*/
void InitHardware(void) {
    /*
      port programming: datasheet pp. 47
    */

    //set data direction: 1: output, 0: input
    DDRB=0b00101011;
    DDRC=0b00101011;
    DDRD=0b01101110;

    TCCR0A=0;
    TCCR0B=0;
}







void timer1A_enable(uint16_t interrupts_per_sec) {
    unsigned char sreg;
    sreg = SREG;
    cli();

    /*
     initialize Timer1 for interrupt driven reading of push-button

     th timer runs at 16000000MHz/1024 = 15.625 kHz
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
       CS12=0 |
       CS11=1 | Clk/64 (Vorteiler)
       CS10=1 |

     TIMSK:
       TOIE1=1/0 => aktiviert/deaktiviert den T/C1-Overflow Interrupr
    */

    cbi(TIMSK1, OCIE1A);

    TCNT1=0;

    TCCR1A=0;
    TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10);


    TCNT1=0;
    OCR1A=(F_CPU/64)/interrupts_per_sec;
    sbi(TIMSK1, OCIE1A);
    SREG = sreg;
}


void timer2A_enable(uint16_t interrupts_per_sec) {
    unsigned char sreg;
    sreg = SREG;
    cli();
    cbi(TIMSK2, OCIE2A);


    TCNT2=0;

    TCCR2A=(1<<WGM21);
    TCCR2B=(1<<CS21); // fclock/8, CTC mode


    TCNT2=0;
    OCR2A=(uint16_t)((uint32_t)(F_CPU/8)/(uint32_t)interrupts_per_sec);
    sbi(TIMSK2, OCIE2A);
    SREG = sreg;
}


uint16_t ADC_read(uint8_t channel, uint8_t measurements, uint16_t wait_us) {
    uint16_t result=0;
    uint8_t i;


    // select given channel
    uint8_t r=channel & 0x0F;
    ADMUX=r;

    // activate ADC and set clock prescaler to 8, i.e. 2MHz
    ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);

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

void to_decimal(char* data, uint8_t decimals) {
    int l=strlen(data);
    for (int i=l-1; i>l-decimals-1; i--) {
        data[i+1]=data[i];
    }
    data[l-decimals]='.';
    if (data[l-decimals-1]==' ') data[l-decimals-1]='0';
    data[l+1]=0;
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
