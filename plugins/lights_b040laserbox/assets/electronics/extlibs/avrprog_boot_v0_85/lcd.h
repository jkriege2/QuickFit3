// Control of a HD44780 compatible LCD in 4-Bit interface mode
// modifoed source code based on:
// <http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/LCD-Ansteuerung>
// <http://www.mikrocontroller.net/articles/DOGMxxx_LCD_Routinen>
// <http://www.lcd-module.de/eng/pdf/zubehoer/st7036.pdf>

#ifndef lcd_h
#define lcd_h


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/pgmspace.h>


// constants to identify different LCD controllers
#define LCD_CONTROLLER_HD44780 0
#define LCD_CONTROLLER_KS0066 1
#define LCD_CONTROLLER_ST7036 2
#define LCD_CONTROLLER_EADOGM LCD_CONTROLLER_ST7036

// define the supply voltage for the LCD (applies to ST7036)
#define LCD_VOLTAGE_5V 0
#define LCD_VOLTAGE_3V 1

// interface defines
#define LCD_INTERFACE_4BIT 0
#define LCD_INTERFACE_8BIT 1
#define LCD_INTERFACE_8BIT_SHIFTREG 2

/** \brief define the controller connected to the AVR
 *
 *  Values that may be used are:
 *     - LCD_CONTROLLER_HD44780 (<a href="http://en.wikipedia.org/wiki/HD44780_Character_LCD">often used standard controller</a>)
 *     - LCD_CONTROLLER_KS0066 (basically HD44780 compatible)
 *     - LCD_CONTROLLER_ST7036 (<a href="http://www.lcd-module.de/eng/pdf/zubehoer/st7036.pdf">ST7036 controller</a>, e.g. used in DOG-M displays from EA)
 *     - LCD_CONTROLLER_EADOGM (<a href="http://www.lcd-module.com/products/dog.html">EA display on glass displays</a>)
 *  .
 */
#define LCD_USED_CONTROLLER LCD_CONTROLLER_EADOGM
/** \brief define the supply voltage (may apply only to some controllers)
 *
 *  Values that may be used are:
 *     - LCD_VOLTAGE_5V
 *     - LCD_VOLTAGE_3V
 *  .
 */
#define LCD_VOLTAGE LCD_VOLTAGE_3V
/** \brief defines which interface to use
 *
 *  Values that may be used are:
 *     - LCD_INTERFACE_4BIT
 *     - LCD_INTERFACE_8BIT
 *     - LCD_INTERFACE_8BIT_SHIFTREG 8-Bit mode, but data is transfered via a shift register connected to LCD_CLK_PIN and LCD_DO_PIN
 *  .
 */
#define LCD_INTERFACE LCD_INTERFACE_8BIT_SHIFTREG

/** \brief define the number of lines of the display (1..4) */
#define LCD_LINECOUNT 3
/** \brief define the number of characters per line (1..80) */
#define LCD_CHARACTERCOUNT 16

/** \brief specify the default contrast value
 *
 * For ST7036 you could use 10 for 5V and 16 for 3,3V operation
 */
#define LCD_DEFAULTCONTRAST 16


/** \brief port (\c PORTx ) used for the data pins */
#define LCD_PORT
/** \brief data direction register (\c DDRx ) used for the data pins */
#define LCD_DDR

/** \brief where are the data lines connected to the port LCD_PORT in 4-Bit mode?
 *
 *  A value of 0 means:
 *    - \c PBx0 is connected to \c D4
 *    - \c PBx1 is connected to \c D5
 *    - \c PBx2 is connected to \c D6
 *    - \c PBx3 is connected to \c D7
 *  .
 *  A value of 1 means:
 *    - \c PBx1 is connected to \c D4
 *    - \c PBx2 is connected to \c D5
 *    - \c PBx3 is connected to \c D6
 *    - \c PBx4 is connected to \c D7
 *  .
 *  ... and so on until a value of 4
 *    - \c PBx4 is connected to \c D4
 *    - \c PBx5 is connected to \c D5
 *    - \c PBx6 is connected to \c D6
 *    - \c PBx7 is connected to \c D7
 *  .
 */
#define LCD_DATASTART 0

/** \brief port to which RS is connected (\c PORTx ) */
#define LCD_RS_PORT   PORTC
/** \brief data direction register for the RS signal (\c DDRx ) */
#define LCD_RS_DDR    DDRC
/** \brief pin for the RS signal (\c Pxy ) */
#define LCD_RS        PC2

/** \brief port to which EN is connected (\c PORTx ) */
#define LCD_EN_PORT   PORTB
/** \brief data direction register for the EN signal (\c DDRx ) */
#define LCD_EN_DDR    DDRB
/** \brief pin for the EN signal (\c Pxy ) */
#define LCD_EN        PB0


/** \brief port to which CLK is connected (\c PORTx ) */
#define LCD_CLK_PORT   PORTC
/** \brief data direction register for the CLK signal (\c DDRx ) */
#define LCD_CLK_DDR    DDRC
/** \brief pin for the CLK signal (\c Pxy ) */
#define LCD_CLK        PC3

/** \brief port to which DO is connected (\c PORTx ) */
#define LCD_DO_PORT   PORTC
/** \brief data direction register for the DO signal (\c DDRx ) */
#define LCD_DO_DDR    DDRC
/** \brief pin for the DO signal (\c Pxy ) */
#define LCD_DO        PC4

/** \brief tells the library whether the STROBE signal is connected (1) to the controller or not (0)
 *
 *  This line may or may not be neccessary, depending on the type of shiftregister used for the communication
 *  with the display.
 *    - 74164 registers won't need a strobe signal
 *    - 4094 registers will need one
 *  .
 */
#define LCD_STROBE_CONNECTED 0
/** \brief port to which DO is connected (\c PORTx ) */
#define LCD_STROBE_PORT   PORTC
/** \brief data direction register for the DO signal (\c DDRx ) */
#define LCD_STROBE_DDR    DDRC
/** \brief pin for the DO signal (\c Pxy ) */
#define LCD_STROBE        PC4

/** \brief tells the library whether the RW signal is connected (1) to the controller or not (0)
 *
 *  RW is only used when data should be read from the LC display. If this is not needed simply
 *  tie the RW pin of the LCD to GND.
 */
#define LCD_RW_CONNECTED 0
/** \brief port to which RW is connected (\c PORTx ) */
#define LCD_RW_PORT   PORTC
/** \brief data direction register for the RW signal (\c DDRx ) */
#define LCD_RW_DDR    DDRC
/** \brief pin for the RW signal (\c Pxy ) */
#define LCD_RW        PC4







/*! \brief send one character to the LCD and (if necessary) transform the character.

 Transformations are performed to make shure that the standard ASCII encoding of a PC
 is best mapped to the character set of the LC display!
 The transformations may depend on the type of the controller and its character set.

 As the lowest 8 characters (0x00..0x07) usually may be used as user defined characters
 but at the same time 0x00 defines the end of a string in C these characters are shifted by 1
 So if you try to print \c '\\x01' this will actually send 0x00, \c '\\x02' will send 0x01 ...

 In addition some characters have a redefined meaning, that allows to set the cursor to one
 of the four available lines:
   - \c '\\f' will set the cursor to the first line
   - \c '\\n' will set the cursor to the second line
   - \c '\\v' will set the cursor to the third line
   - \c '\\r' will set the cursor to the fourth line
 .

 */
void lcd_putc(char data);

/** \brief send one character to the LCD, without transformations \see lcd_putc() */
void lcd_putc_raw(char data);

/** \brief send a string doing transformations */
void lcd_string(char *data);

/** \brief send a command */
void lcd_command(unsigned char temp1);

/** \brief init the display
 *
 *  \param cursor_onoff switch the cursor on or off
 *  \param cursor_blinking should the cursor be blinking?
 *
 */
void lcd_init(char cursor_onoff, char cursor_blinking);

/** \brief configure cursor and switch display on/off
 *
 *  \param display_onoff switch the display on (!=0) or off (0)
 *  \param cursor_onoff switch the cursor on (!=0) or off (0)
 *  \param cursor_blinking should the cursor be blinking?
 */
void lcd_setcursor(char display_onoff, char cursor_onoff, char cursor_blinking);

/** \brief configure double height lines on displays with >1 lines
 *
 *  \param doubleheight_line position of double height line (<=0 : none, =1: line 1+2 are combined, =2: line 2+3 are combined)
 *
 */
void lcd_setdoubleheight(char doubleheight_line);

/** \brief move the cursor home (i.e. to the first character in the first line */
void lcd_home(void);

/** \brief clear the contents of the display */
void lcd_clear(void);

/** \brief move the cursor to a specified location
 *
 *  \param x x-position in line
 *  \param y the line to write into [1..4]
 */
void lcd_cursor(uint8_t x, uint8_t y);

/*! \brief set the contrast of the display.

    \note This function only applies to displays that support setting the contrast by software.
          An example is the DOG-M displays from EA which use the ST7036 controller */
void lcd_setcontrast(uint8_t contrast);

/*! \brief in principle the same as lcd_putc(), but compatible with avr-libc's FILE interface

    You can use this method to set the display as receiver of a FILE stream of the avr-libc,
    so you can use fprintf to output data to the display:
    \code
LCD=fdevopen(lcd_write, NULL);          //LCD-Ausgabe initialisieren
fprintf(LCD, "\fline 1\nand line 1+1=%d", 1+1);
    \endcode
 */
int lcd_write(char data, FILE *stream);

/*! \brief this may be used to set new characters to the CGRAM:

     \param id write the id-th CGRAM character
     \param data representation of the character
 */
void lcd_define_letter(uint8_t id, uint8_t* data);

/** \brief same as lcd_define_letter(), but with data from FLASH memory */
void lcd_define_letter_P(uint8_t id, PGM_VOID_P data);

/** \brief send a string to line 1 */
#define lcd_string_line1(data) { lcd_cursor(0,1); lcd_string(data); }

/** \brief send a string to line 2 */
#define lcd_string_line2(data) { lcd_cursor(0,2); lcd_string(data); }

/** \brief send a string to line 3 */
#define lcd_string_line3(data) { lcd_cursor(0,3); lcd_string(data); }

/** \brief send a string to line 4 */
#define lcd_string_line4(data) { lcd_cursor(0,4); lcd_string(data); }

/** \brief write starting from a specified position
 *
 *  \param x x-position in line
 *  \param y the line to write into [1..4]
 *  \param data the string to write
 */
#define lcd_pos_string(x, y, data) { lcd_cursor(x, y); lcd_string(data); }






#endif /* lcd_h */


