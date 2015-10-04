#include "tools.h"



/*
  InitHardware() initializes the MC
*/
void InitHardware(void) {
    /*

    PWM_FAN             		PD7 (o)
    SW_ONOFF    	        	PD6 (i)
    SWLED                       PD5 (o)
    SW_MENU				        PD4 (i)
    SW_DOWN				        PD3 (i)
    SW_UP				        PD2 (i)
    TxD                         PD1 (o)
    RxD                         PD0 (i)

      LCD:                        PC1-PC7 (o)

    SCK                         PB7 (o)
    MISO                        PB6 (i)
    MOSI                        PB5 (o)
    CS_DAC                      PB4 (o)
    LCD_BACK                    PB3 (o)
    CS_ADC                      PB2 (o)
    TTL_OUT                     PB1 (o)

    switch Output on/off        PA4 (o)
    Ucurrent                    PA5 (ai)
    Utemp                       PA6 (ai)



      port programming: datasheet pp. 47
    */

    //set data direction: 1: output, 0: input
    DDRA=0b00010000;
    DDRB=0b10111110;
    DDRC=0b11111101;
    DDRD=0b10100010;


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


void PWM0_enable(void) {
    /*
      timer 0 is used for PWM output for the LCD backlight

    - prescaler: divide by 1
    - fast PWM mode


    WGM01=1
    WGM00=1
    COM01=1
    COM00=0
    CS02=0
    CS01=0
    CS00=1
     */
    TCCR0=(1<<WGM01)|(1<<WGM00)|(1<<COM01)|(1<<CS00);
    TCNT0=0;
    OCR0=0;
}

void PWM0_disable(void) {
    TCCR0=0;
}

void PWM0_set(uint8_t value) {
    OCR0=value;
}



void PWM2_enable(void) {
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
    TCCR2=(1<<WGM21)|(1<<WGM20)|(1<<COM21)|(1<<CS20);
    TCNT2=0;
    OCR2=0;
}

void PWM2_disable(void) {
    TCCR2=0;
}

void PWM2_set(uint8_t value) {
    OCR2=value;
}


void timer1_enable(void) {
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

    TIMSK=0; //all timer interrupts disabled, T/C interrupt mask register f

    TCNT1=0;

    TCCR1A=0;
    TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10);


    TCNT1=0;
    //OCR1A=1563;
    OCR1A=781;
    sbi(TIMSK, OCIE1A);
    SREG = sreg;
}

void timer1B_enable(void) {
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

    TIMSK=0; //all timer interrupts disabled, T/C interrupt mask register f

    TCNT1=0;

    TCCR1A=0;
    TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10);


    TCNT1=0;
    //OCR1B=1563;
    OCR1B=781;
    sbi(TIMSK, OCIE1B);
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


void DAC_set(uint16_t value) {
    CS_DAC();
    uint16_t d=value&0x0FFF;
    d=d|0b0111000000000000;
    uint8_t dd=(d>>8)&0x00FF;
    spi_write(dd);
    dd=d&0x00FF;
    spi_write(dd);
    CS_none();
}

int16_t extADC_read(uint8_t measurements, uint16_t wait_us) {
	uint16_t result=0;
	for (int i=0; i<measurements; i++)
	{
		CS_ADC();	//chip select line low (active)
		uint8_t receive1=spi_readwrite(0) & 0b00011111;
		uint8_t receive2=spi_readwrite(0);
		int16_t res;

		res=receive2 + (receive1<<8);

		// to deal with 13 bit 2's-Complement numbers: fill up the first 3 bit
		// in the resulting 16 bit number with the Sign-Bit of the 13-bit number (bit 13)
		// explanation at http://en.wikipedia.org/wiki/Two%27s_complement
        if (res>4095) res=res|0xF000;
		result += res;
		CS_none(); //deactivate chip select line
		if (wait_us>0) _delay_us(wait_us);
	}
	result /= measurements;
	return result;
}

