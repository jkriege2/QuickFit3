/*
  this file:     Tool functions
  project:       TEC Controller
  author:        Jan Soldat
                 jan.soldat@gmail.com

  target system:     Atmel ATmega32, no external RAM
                     XTAL: 16 MHz
  compiler (tested): avr-gcc 4.3.3
*/

#include "tools.h"
#include <string.h>


void timer0_enable(void){
   unsigned char sreg;
   sreg = SREG;
   cli();

   /*
    initialize Timer0 for interrupt driven reading of push-button

    the timer runs at 16MHz/1024 = 15,625 kHz
    ==> count from 0 to 255. On counter=255 ther will be a compare match,
	    this makes about 61 overflow interrupts per second. Use a global variable as counter
		from 0 to 6 to read out buttons 10 times per second
	*/

	//non PWM mode, Clk/1024
	TCCR0 |= (1<<CS02) | (1<<CS00);
	OCR0 = 255;

	TIMSK |= (1<<TOV0);

	SREG = sreg;
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
       CS12=1 |
       CS11=0 | Clk/256 (Vorteiler)
       CS10=0 |

     TIMSK:
       TOIE1=1/0 => aktiviert/deaktiviert den T/C1-Overflow Interrupr
    */

    cbi(TIMSK, OCIE1A);

    TCNT1=0;

    TCCR1A=0;
    TCCR1B=(1<<WGM12)|(1<<CS12);//|(1<<CS10);


    TCNT1=0;
    //OCR1A=1563;
    OCR1A=(F_CPU/256)/interrupts_per_sec;
    sbi(TIMSK, OCIE1A);
    SREG = sreg;
}


//sends a character on the SPI interface
uint8_t spi_write(unsigned char data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}


//reads out kty81/220 temperature sensor via internal ADC
//taking the average of <quantity> measurements
uint16_t read_inttemp(uint8_t quantity)
{
	uint16_t result=0;
	ADCSRA |= (1<<ADSC);

	for (uint8_t a=0; a < quantity; a++)
	{
		ADCSRA |= (1<<ADSC);
		while(bit_is_set(ADCSRA,ADSC)) {}	//wait for conversion to complete

		result += ADCL;
		result += (ADCH << 8);
//		int_temp = (int)(adc_result - 264)/1.68;	//alte Berechnung
	}
	result /= quantity;	//take the average of the <quantity> measurements

	return result;
}

//reads the MCP3208 ADC <quantity> times on ch. <channel>
uint16_t read_adc(uint8_t channel, uint8_t quantity)
{
	uint32_t result=0;
	for (int i=0; i<quantity; i++)
	{
		cs_adc();	//chip select line low (active)
		uint8_t data=0b00000110 | ((channel & 0b00000100) >> 2); //write config bits and upper channel bit
		uint8_t receive=spi_write(data);
		data=(channel & 0b00000011) << 6;
		receive=spi_write(data); //write lower 2 channel bits and receive upper 4 result bits
		receive &= (0b00001111);
		result += (receive << 8);
		receive=spi_write(255); //write dummy data and receive lower 8 result bits
		result += receive;
		cs_none(); //deactivate chip select line
		_delay_us(100);
	}
	result /= quantity;

	return result;
}

//reads the MCP3208 ADC <quantity> times on ch. <channel>
uint16_t read_adc_single(uint8_t channel)
{
    uint32_t result=0;
    cs_adc();	//chip select line low (active)
    uint8_t data=0b00000110 | ((channel & 0b00000100) >> 2); //write config bits and upper channel bit
    uint8_t receive=spi_write(data);
    data=(channel & 0b00000011) << 6;
    receive=spi_write(data); //write lower 2 channel bits and receive upper 4 result bits
    receive &= (0b00001111);
    result += (receive << 8);
    receive=spi_write(255); //write dummy data and receive lower 8 result bits
    result += receive;
    cs_none(); //deactivate chip select line

	return result;
}

void avr_init(void)
{
    //DDRD |= /*(1<<DDD7) | (1<<DDD6) |*/ (1<<DDD5) /*| (1<<DDD3)*/ | (1<<DDD2);
    //DDRC |= (1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) /*| (1<<DDC3) | (1<<DDC2)*/ | (1<<DDC0);
	//DDRA &= ~(1<<DDA7);

	DDRA=0b00111111;
	DDRB=0b10110000;
	DDRC=0b11111101;
	DDRD=0b11111110;

	//PORTD &= ~(1<<PORTD7);
	//PORTD &= ~(1<<PORTD6);
	//PORTD |= (1<<PORTD1); //UART TxD

//	intfan_off();
	led_1_off();
	led_2_on();
	lcdback_on();

	//cs_none();

	//lcd_init(LCD_DISP_ON);  //turn on LCD

	//SPI Init
	//DDRB |= (1<<DDB5); //MOSI Output
	//DDRB &= ~(1<<DDB6); //MISO Input
	//DDRB |= (1<<DDB7); //SCK Output
	// SS\ wird von lcd_init() auf Output gesetzt -> Master Mode

	SPCR |= (1<<SPE) | (1<<MSTR) | (1<<SPR1);
	SPSR |= (1<<SPI2X);
	//Enable, MSB first, Master mode, rising edge = leading, leading edge=sample, clock rate 500 kHz
	//SPI Init Ende

	//uart_init(9600, 8, 1, UART_NOPARITY);
	//uart_init();

	//Internal ADC Init
	ADMUX |= (1<<MUX2) | (1<<MUX1) | (1<<MUX0);
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	ADCSRA |= (1<<ADEN);


	//set DAC for Vctli control: no TEC current
	//set_dac_4921(2048);

}



       /*
            Pt100 Readout and conversion to °C
            ----------------------------------

            Vref = 2.5 V
            R = 1240 Ohm
            => I = 2,02 mA SET_TEMP through Pt100

            Resistance of Pt100 is given by
            R(T) = R0*(1+a*T+b*T^2)
            where a = 3.9083 E-3
                  b = -5.775 E-7

            approximated linear function:
            R(T) = R0*(1+d*T)
            where d = 3.85 E-3

            Voltage drop at Pt100 in terms of ADC units (12 Bit resolution => 4096 units):
            (factor 10: amplification)
            ADC/4096 * 3 V / 10 = I * R(T)
            ADC/4096 * 3 V / 10 = 2.02e-3 * 100*(1+3.85 E-3*T)

            => ADC * 3/40960 = 0.202 + 7.777e-4 * T
               ADC * 3 = 8273.9 + 31.854 * T
               T = ( 3*ADC - 8274 ) / 31.854
                 = ( 30 * ADC - 82740 ) / 318.54

            use another factor of 10 for the 1/10 °C:
               T = ( 300 * ADC - 827400 ) / 318.54


        */
int16_t PT1002T(uint16_t adc, int16_t PT100_OFFSET) {
    int32_t a=adc;
    if ((a>100)&&(a<4090)) {
        a=a*(int32_t)300;
        a=a-(int32_t)827400;
        a=a*10/(int32_t)3185;
        if (a>10000) a=10000;
        if (a<-10000) a=-10025;
        return a+PT100_OFFSET; // empirical offset correction in software!!!
    } else return 10000;
}




       /*
            Pt100 Readout and conversion to °C
            ----------------------------------

            Vref = 2.5 V
            R = 1240 Ohm
            => I = 2,02 mA SET_TEMP through Pt100

            Resistance of Pt100 is given by
            R(T) = R0*(1+a*T+b*T^2)
            where a = 3.9083 E-3
                  b = -5.775 E-7

            approximated linear function:
            R(T) = R0*(1+d*T)
            where d = 3.85 E-3

            Voltage drop at Pt100 in terms of ADC units (12 Bit resolution => 4096 units):
            (factor 10: amplification)
            ADC/4096 * 3 V / 10 = I * R(T)
            ADC/4096 * 3 V / 10 = 2.02e-3 * 100*(1+3.85 E-3*T)

            => ADC * 3/40960 = 0.202 + 7.777e-4 * T
               ADC * 3 = 8273.9 + 31.854 * T
               T = ( 3*ADC - 8274 ) / 31.854
                 = ( 30 * ADC - 82740 ) / 318.54

            use another factor of 100 for the 1/100 °C:
               T = ( 3000 * ADC - 8274000 ) / 318.54

        */
int32_t PT1002T_100thdeg(uint16_t adc, int16_t PT100_OFFSET) {
    int32_t a=adc;
    if ((a>100)&&(a<4090)) {
        a=a*(int32_t)3000;
        a=a-(int32_t)8274000;
        a=(a*10)/(int32_t)3185;
        if (a>100000) a=100000;
        if (a<-100000) a=-100250;
        return a+PT100_OFFSET; // empirical offset correction in software!!!
    } else return 100000;
}

void to_decimal(char* data, uint8_t decimals) {
    if (decimals<=0) return;
    int l=strlen(data);
    for (int i=l-1; i>l-decimals-1; i--) {
        data[i+1]=data[i];
    }
    data[l-decimals]='.';
    if (data[l-decimals-1]==' ') data[l-decimals-1]='0';
    data[l+1]=0;
}



void PWM2_enable(uint8_t prescaler) {
    /*
      timer 2 is used for PWM output

    - prescaler: divide by N (1: prescaler=1
                              8: prescaler=2
                              32: prescaler=3
                              64: prescaler=4
                              128: prescaler=5
                              256: prescaler=6
                              1024: prescaler=7
                              )
    - phase-corrected PWM mode
    - Clear OC2 on compare match when up-counting. Set OC2 on compare match when downcounting.

     */
    TCCR2=(1<<WGM20)|(1<<COM21)|(prescaler&0b00000111);
    TCNT2=0;
    OCR2=0;
}

void PWM2_disable(void) {
    TCCR2=0;
}

void PWM2_set(uint8_t value) {
    OCR2=value;
    if (value==0) {
        cbi(TCCR2, COM21);
        cbi(TCCR2, COM20);
        cbi(PORTD, PD7);
    } else {
        sbi(TCCR2, COM21);
        cbi(TCCR2, COM20);
    }
}





/*
MC Pin Configuration

LEDs:
LED 1				PD 2
LED 2				PC 5

LCD:
D7					PA 3 (MSB)
D6					PA 2
D5					PA 1
D4					PA 0
Enable				PA 4
R/W					PA 5
RS					PB 4
Backlight			PC 0

Schalter:
S 2					PB 3
S 3					PB 2
S 4					PB 1
S 5					PB 0

Temperaturmessung:
KTY81/220			PA 7

fans:
internal fan		PD 5

TEC:
TEC Shutdown		PC 2
DC On/Off			PC 3
TEC Multiplexer B	PD 7
TEC Multiplexer A	PD 6
TEC Mp Enable\		PD 3


ADC MCP3208
SCK					PB 7
MISO				PB 6
MOSI				PB 5
ADC_CS\ Shutdown	PC 4

UART
RxD					PD 0
TxD					PD 1
*/





