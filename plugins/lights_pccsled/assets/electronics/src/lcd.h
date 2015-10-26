// Control of a HD44780 compatible LCD in 4-Bit interface mode
// modofoed source code based on:
// <http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung>

#ifndef lcd_h
#define lcd_h


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/pgmspace.h>

void lcd_data(unsigned char temp1);
void lcd_string(char *data);
void lcd_command(unsigned char temp1);
void lcd_enable(void);
void lcd_init(void);
void lcd_home(void);
void lcd_clear(void);
void lcd_cursor(uint8_t x, uint8_t y);

// output data to LCD (may be used in conjunction with printf on a output stream)
// \f sets the cursor to line 1
// \n sets the cursor to line 2
// \b sets the cursor to line 4
// \r sets the cursor to line 3
int lcd_write(char data, FILE *stream);

// this may be used to set new characters to the CGRAM:
// give data for the id-th CGRAM character. <data> is a 8 entry array
void lcd_define_letter(uint8_t id, uint8_t* data);
// same as above but with data in programspace
void lcd_define_letter_P(uint8_t id, PGM_VOID_P data);


#define lcd_string_line1(data) { lcd_cursor(0,1); lcd_string(data); }
#define lcd_string_line2(data) { lcd_cursor(0,2); lcd_string(data); }
#define lcd_pos_string(x, y, data) { lcd_cursor(x, y); lcd_string(data); }


// Hier die verwendete Taktfrequenz in Hz eintragen, wichtig!

// LCD instructions

#define CLEAR_DISPLAY 0x01
#define CURSOR_HOME   0x02

// pins used for LCD
#define LCD_PORT      PORTC
#define LCD_DDR       DDRC
#define LCD_RS        PC2
#define LCD_EN        PC3
//#define LCD_RW        PC2
#undef LCD_RW

// set to 0 if data bits start at pin 0 and 4 if data bits start at pin 4
#define LCD_DATASTART 4

#define LCD_LONGDELAY 1700
#define LCD_SHORTDELAY 45



#endif /* lcd_h */


