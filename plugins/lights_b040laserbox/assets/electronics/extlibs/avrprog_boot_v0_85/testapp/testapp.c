// Martin Thomas 4/2008

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define LED_PORT   PORTB
#define LED_DDR    DDRB
#define LED_BIT    PB2

#define BT_PORT    PORTC
#define BT_DDR     DDRC
#define BT_PIN     PINC
#define BT_BIT     PC7

static inline void my_wdt_off(void)
{
	cli();
	wdt_reset();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Write logical one to WDCE and WDE */
	/* Keep old prescaler setting to prevent unintentional time-out */
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Turn off WDT */
	WDTCSR = 0x00;
}

static void my_delay( void )
{
	int i;
	for ( i=0; i<500; i++ ) {
		_delay_ms(1);
	}
}

int main( void ) 
{
	my_wdt_off();

	LED_PORT |=  ( 1<<LED_BIT );
	LED_DDR  |=  ( 1<<LED_BIT );
	
	BT_DDR   &= ~( 1<<BT_BIT );
	BT_PORT  |=  ( 1<<BT_BIT );

	while ( 1 ) {
		LED_PORT &= ~( 1<<LED_BIT );
		my_delay();
		LED_PORT |=  ( 1<<LED_BIT );
		my_delay();
		
		if ( !( BT_PIN & ( 1<<BT_BIT ) ) ) {
			/* start bootloader */
			wdt_enable( WDTO_250MS );
			while (1) { ; }
		}
	}

	return 0; /* never reached */
}
