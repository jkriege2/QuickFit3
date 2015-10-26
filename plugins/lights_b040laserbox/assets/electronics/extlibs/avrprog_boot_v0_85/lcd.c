#include "lcd.h"





// set timing constants
#define LCD_LONGDELAY 1700
#define LCD_SHORTDELAY 45
#define LCD_DELAY_PRECLOCK 30
#define LCD_CLOCKDELAY 5

#if LCD_USED_CONTROLLER == LCD_CONTROLLER_ST7036
    #define LCD_LONGDELAY 500
    #define LCD_SHORTDELAY 40
    #define LCD_DELAY_PRECLOCK 5
    #define LCD_CLOCKDELAY 5
#endif





#define sbi(port, pin) port|=_BV(pin)
#define cbi(port, pin) port&= ~(_BV(pin))

// some preprocessor magic ;-)
#define RS1() LCD_RS_PORT|=_BV(LCD_RS);
#define RS0() LCD_RS_PORT&=~(_BV(LCD_RS));

#define EN1() LCD_EN_PORT|=_BV(LCD_EN);
#define EN0() LCD_EN_PORT&=~(_BV(LCD_EN));


#define CLK1() LCD_CLK_PORT|=_BV(LCD_CLK);
#define CLK0() LCD_CLK_PORT&=~(_BV(LCD_CLK));

#define DO1() LCD_DO_PORT|=_BV(LCD_DO);
#define DO0() LCD_DO_PORT&=~(_BV(LCD_DO));

#if (LCD_RW_CONNECTED != 0)
    #define RW1() LCD_RW_PORT|=_BV(LCD_RW);
    #define RW0() LCD_RW_PORT&=~(_BV(LCD_RW));
#else
    #define RW1()
    #define RW0()
#endif

#if (LCD_INTERFACE == LCD_INTERFACE_4BIT)
# define LCDPORTS_IN()  LCD_DDR &= ~(0x0F<<LCD_DATASTART);
# define LCDPORTS_OUT() { LCD_DDR |= 0x0F<<LCD_DATASTART; }
#elif (LCD_INTERFACE == LCD_INTERFACE_8BIT)
# define LCDPORTS_IN()  LCD_DDR = 0x00;
# define LCDPORTS_OUT() LCD_DDR = 0xFF;
#elif (LCD_INTERFACE == LCD_INTERFACE_8BIT_SHIFTREG)
# define LCDPORTS_IN()  sbi(LCD_DO_DDR, LCD_DO); sbi(LCD_CLK_DDR, LCD_CLK)
# define LCDPORTS_OUT() sbi(LCD_DO_DDR, LCD_DO); sbi(LCD_CLK_DDR, LCD_CLK)
# if (LCD_STROBE_CONNECTED != 0)
#  define LCDPORTS_OUT() sbi(LCD_DO_DDR, LCD_DO); sbi(LCD_CLK_DDR, LCD_CLK); sbi(LCD_STROBE_DDR, LCD_STROBE)
# endif
#endif
// create a short pulse on EN line
#define lcd_enable() \
    EN0(); \
    _delay_us(LCD_DELAY_PRECLOCK); \
    EN1(); \
    _delay_us(LCD_CLOCKDELAY); \
    EN0();


// here some controller specific constants are set
#define LCD_INSTRUCTION_CLEAR    0x01
#define LCD_INSTRUCTION_HOME     0x02

#define LCD_INSTRUCTION_GOTO     0x80

#define LCD_BASEADRESS_LINE1     0x00
#define LCD_BASEADRESS_LINE2     0x40
#define LCD_BASEADRESS_LINE3     0x10
#define LCD_BASEADRESS_LINE4     0x50

#if (LCD_LINECOUNT == 1)
    // redirect all output to line 1
    #define LCD_BASEADRESS_LINE2     0x00
    #define LCD_BASEADRESS_LINE3     0x00
    #define LCD_BASEADRESS_LINE4     0x00
#elif (LCD_LINECOUNT == 2)
    #define LCD_BASEADRESS_LINE1     0x00
    #define LCD_BASEADRESS_LINE2     0x40
    #define LCD_BASEADRESS_LINE3     0x00
    #define LCD_BASEADRESS_LINE4     0x40
#elif (LCD_LINECOUNT == 3)
    #define LCD_BASEADRESS_LINE1     0x00
    #define LCD_BASEADRESS_LINE2     0x10
    #define LCD_BASEADRESS_LINE3     0x20
    #define LCD_BASEADRESS_LINE4     0x00
#elif (LCD_LINECOUNT == 4)
    #define LCD_BASEADRESS_LINE1     0x00
    #define LCD_BASEADRESS_LINE2     0x40
    #define LCD_BASEADRESS_LINE3     0x10
    #define LCD_BASEADRESS_LINE4     0x50
#endif


// send 8 data bits to LCD
void lcd_data_out(unsigned char temp1) {

#if (LCD_INTERFACE == LCD_INTERFACE_4BIT)
    #warning "                 4 Bit Interface"
    // 1. transmit high nibble
    unsigned char temp2=temp1 >> 4;
    LCD_PORT &= ~(0x0F << LCD_DATASTART); // clear databits of port
    LCD_PORT |= temp2<<LCD_DATASTART; // output low nibble
    /*if (temp1 & 0b10000000) sbi(LCD_PORT, 3); else cbi(LCD_PORT, 3);
    if (temp1 & 0b01000000) sbi(LCD_PORT, 2); else cbi(LCD_PORT, 2);
    if (temp1 & 0b00100000) sbi(LCD_PORT, 1); else cbi(LCD_PORT, 1);
    if (temp1 & 0b00010000) sbi(LCD_PORT, 0); else cbi(LCD_PORT, 0);*/
    lcd_enable(); // pulse EN line

    // 2. transmit low nibble
    temp2=(temp1 & 0x0F);
    LCD_PORT &= ~(0x0F << LCD_DATASTART); // clear databits of port
    LCD_PORT |= temp2<<LCD_DATASTART; // output low nibble
    /*if (temp1 & 0b00001000) sbi(LCD_PORT, 3); else cbi(LCD_PORT, 3);
    if (temp1 & 0b00000100) sbi(LCD_PORT, 2); else cbi(LCD_PORT, 2);
    if (temp1 & 0b00000010) sbi(LCD_PORT, 1); else cbi(LCD_PORT, 1);
    if (temp1 & 0b00000001) sbi(LCD_PORT, 0); else cbi(LCD_PORT, 0);*/
#elif (LCD_INTERFACE == LCD_INTERFACE_8BIT)
    #warning "                 8 Bit Interface"
    LCD_PORT = temp1;
#elif (LCD_INTERFACE == LCD_INTERFACE_8BIT_SHIFTREG)
    #warning "                 8 Bit Interface with ShiftReg"
    uint8_t bit=0b10000000;
    cbi(LCD_CLK_PORT, LCD_CLK);
    _delay_us(LCD_CLOCKDELAY);
    uint8_t cnt=0;
    while (cnt<8) {//>0) {
        if ((temp1 & bit) !=0) {
            sbi(LCD_DO_PORT, LCD_DO);
        } else {
            cbi(LCD_DO_PORT, LCD_DO);
        }

        sbi(LCD_CLK_PORT, LCD_CLK);
        _delay_us(LCD_CLOCKDELAY);
        cbi(LCD_CLK_PORT, LCD_CLK);
        _delay_us(LCD_CLOCKDELAY);

        bit=bit>>1;
        //bit=bit<<1;
        cnt++;
    }
    #if (LCD_STROBE_CONNECTED != 0)
        #warning "                 ShiftReg uses STROBE signal"
        _delay_us(LCD_CLOCKDELAY);
        sbi(LCD_STROBE_PORT, LCD_STROBE);
        _delay_us(LCD_CLOCKDELAY);
        cbi(LCD_STROBE_PORT, LCD_STROBE);
    #endif
#endif
    lcd_enable(); // pulse EN line

}

// sned only the lower 4 bit to LCD ... only useful in 4Bit mode
void lcd_data_out4(unsigned char temp1) {

#if (LCD_INTERFACE == LCD_INTERFACE_4BIT)
    // 1. transmit high nibble
    unsigned char temp2=(temp1 & 0x0F);
    LCD_PORT &= ~(0x0F << LCD_DATASTART); // clear databits of port
    LCD_PORT |= temp2<<LCD_DATASTART; // output low nibble
    lcd_enable(); // pulse EN line
#endif

}

void lcd_data(unsigned char temp1) {
    RW0();        // set RW=0
    RS1();        // set RS=1
    lcd_data_out(temp1);
    //lcd_data_out(0b10110100);
    _delay_us(LCD_SHORTDELAY);
}

// sendet einen Befehl an das LCD

void lcd_command(unsigned char temp1) {
    RW0();
    RS0();
    lcd_data_out(temp1);
    //lcd_data_out(0b10110100);
    _delay_us(LCD_SHORTDELAY);
}


// Initialisierung:
// Muss ganz am Anfang des Programms aufgerufen werden.

void lcd_init(char cursor_onoff, char cursor_blinking) {
#if (LCD_USED_CONTROLLER==LCD_CONTROLLER_KS0066) || (LCD_USED_CONTROLLER==LCD_CONTROLLER_HD44780)
    #warning "                 LCD library uses KS0066 or HD44780 controller"
    LCDPORTS_OUT();
    LCD_RS_DDR |= (1<<LCD_RS);
#if LCD_RW_CONNECTED!=0
    LCD_RW_DDR |= (1<<LCD_RW);
#endif
    LCD_EN_DDR |= (1<<LCD_EN);
    _delay_ms(40);

    // muss 3mal hintereinander gesendet werden zur Initialisierung

    RW0();        // set RW=0
    RS0();        // RS auf 0 setzen
    EN0();        // RS auf 0 setzen
    _delay_ms(40);

    if (LCD_DATASTART==0) {
        LCD_PORT &= 0xF0;
        LCD_PORT |= 0x03;               // setzen
    } else {
        LCD_PORT &= 0x0F;
        LCD_PORT |= (0x03<<4);               // setzen
    }

    LCD_RS_PORT &= ~(1<<LCD_RS);      // RS auf 0
    lcd_enable();

    _delay_ms(1);
    lcd_enable();

    _delay_ms(5);
    lcd_enable();
    _delay_ms(5);

    // 4 Bit Modus aktivieren
    if (LCD_DATASTART==0) {
        LCD_PORT &= 0xF0;
        LCD_PORT |= 0x02;               // setzen
    } else {
        LCD_PORT &= 0x0F;
        LCD_PORT |= (0x02<<4);               // setzen
    }
    lcd_enable();
    _delay_ms(5);

    // 4Bit / 2 Zeilen / 5x7
    lcd_command(0x28);

    // Display ein / Cursor aus / kein Blinken
    lcd_command(0x0C);

    // inkrement / kein Scrollen
    lcd_command(0x06);

    lcd_clear();
#elif (LCD_USED_CONTROLLER==LCD_CONTROLLER_ST7036)
    #warning "                 LCD library uses ST7036 controller"
    // set 4 data ports as outputs and all control signals as outputs
    LCDPORTS_OUT();
    LCD_RS_DDR |= (1<<LCD_RS);
    LCD_RW_DDR |= (1<<LCD_RW);
    LCD_EN_DDR |= (1<<LCD_EN);
    _delay_ms(20);

    RW0();
    RS0();
    EN0();
    _delay_ms(100);

    #if (LCD_INTERFACE == LCD_INTERFACE_4BIT)
        #warning "                 4 Bit Interface"
            // init 4-bit by sending 0b0011XXXX three times to the display
            lcd_data_out(0x03);
            _delay_ms(10);
            //lcd_enable();
            lcd_data_out(0x03);
            _delay_ms(10);
            lcd_data_out(0x03);
            _delay_ms(10);

            lcd_data_out(0x02); // function set 0b0010XXXX
            _delay_ms(10);
    #elif (LCD_INTERFACE == LCD_INTERFACE_8BIT || LCD_INTERFACE == LCD_INTERFACE_8BIT_SHIFTREG)
        #warning "                 8 Bit Interface"
            lcd_data_out(0x39);
            _delay_us(LCD_SHORTDELAY);
            lcd_data_out(0x39);
            _delay_us(LCD_SHORTDELAY);
    #endif
    uint8_t c=0x31;
    #if (LCD_VOLTAGE == LCD_VOLTAGE_5V)
        #warning "                 5V supply voltage"
        #if (LCD_LINECOUNT>1)
        c=c + 0x08;
        #endif
        lcd_command(c);    // function set        0b0011<N><DH><IS2><IS1>
                           //                     0b0011 N  0   0    1    =0x31 & (N*0x08)
        lcd_command(0x1D); // intern. osc freq.   0b0001<BS><F2><F1><F0>
                           //                     0b0001 1   1   0   1    =0x1D
        lcd_command(0x50); // power/ICON/contrast 0b0101<Ion><Bon><C5><C4>
                           //                     0b0101 0    0    0   0    =0x50
        lcd_command(0x6C); // follower control    0b0110<Fon><Rab2><Rab1><Rab0>
                           //                     0b0110  1    1     0     0    =0x6C
        lcd_command(0x7C); // contrast set        0b0111<C3><C2><C1><C0>
                           //                     0b0111 1   1   0   0    =0x7C
    #elif (LCD_VOLTAGE == LCD_VOLTAGE_3V)
        #warning "                 3V supply voltage"
        #if (LCD_LINECOUNT>1)
        c=c + 0x08;
        #endif
        lcd_command(c);    // function set        0b0011<N><DH><IS2><IS1>
                           //                     0b0011 N  0   0    1    =0x31 & (N*0x08)
        lcd_command(0x15); // intern. osc freq.   0b0001<BS><F2><F1><F0>
                           //                     0b0001 0   1   0   1    =0x15
        lcd_command(0x55); // power/ICON/contrast 0b0101<Ion><Bon><C5><C4>
                           //                     0b0101 0    1    0   1    =0x55
        lcd_command(0x6E); // follower control    0b0110<Fon><Rab2><Rab1><Rab0>
                           //                     0b0110  1    1     1     0    =0x6E
        lcd_command(0x72); // contrast set        0b0111<C3><C2><C1><C0>
                           //                     0b0111 0   0   1   0    =0x72
    #endif

    lcd_command(c & 0b11111100); // switch back to instruction table 0  0b0011<N>000  =0x38


#endif
    lcd_setcursor(1, cursor_onoff, cursor_blinking);
    lcd_setcontrast(LCD_DEFAULTCONTRAST);
    lcd_clear();
    lcd_home();
}

void lcd_setcursor(char display_onoff, char cursor_onoff, char cursor_blinking) {
    uint8_t b=0b00001000;
    if (display_onoff!=0) b=b+0x04;
    if (cursor_onoff!=0) b=b+0x02;
    if (cursor_blinking!=0) b=b+0x01;
    lcd_command(b); // switch on display and set cursor and blinking of cursor 0b000011<C><B>
}

void lcd_setdoubleheight(char doubleheight_line) {
#if (LCD_LINECOUNT>1)
    #if (LCD_LINECOUNT==2)
        // function set        0b0011<N><DH><IS2><IS1>
        //                     0b0011 1  DH  0    0    =0x38 or 0x3C
        //
        // - set the instruction table to 0,
        // - N=1 as double height fonts only make sense for 2- or 3-line displays.
        //
        if (doubleheight_line==1) { // DH=1, N=1, UD=0
            lcd_command(0x3C);
        } else { // DH=0, N=1, UD=0
            lcd_command(0x38);
        }
    #else
        // function set        0b0011<N><DH><IS2><IS1>
        //                     0b0011 1  DH  0    0    =0x38 or 0x3C
        // function set        0b0011<N><DH><IS2><IS1>
        //                     0b0011 1  DH  1    0    =0x3A or 0x3E
        //
        // - IS2=0, IS1=0: set the instruction table to 0,
        // - IS2=1, IS1=0: switch to function table 2, where the UD bit is available
        // - N=1 as double height fonts only make sense for 2- or 3-line displays.
        //
        // double height position select:  0b0001<UD>000 = 0x10 or 0x18
        if (doubleheight_line==1) { // DH=1, N=1, UD=1
            lcd_command(0x3E); // set DH and switch to table 10
            lcd_command(0x18); // set UD
            lcd_command(0x3C); // set DH and switch to table 00
        } else if (doubleheight_line==2) { // DH=1, N=1, UD=0
            lcd_command(0x3E); // set DH and switch to table 10
            lcd_command(0x10); // clear UD
            lcd_command(0x3C); // set DH and switch to table 00
        } else { // DH=0, N=1, UD=0
            lcd_command(0x3A); // clear DH and switch to table 10
            lcd_command(0x10); // clear UD
            lcd_command(0x38); // clear DH and switch to table 00
        }
    #endif
#endif
}


void lcd_setcontrast(uint8_t contrast) {
#if (LCD_USED_CONTROLLER==LCD_CONTROLLER_ST7036)
    uint8_t c=contrast&0b00111111;
    uint8_t c1=(c>>4)&0x03;

    lcd_command(0x39);    // function set        0b0011<N><DH><IS2><IS1>
                          //                     0b0011 1  0   0    1    =0x39
    #if (LCD_VOLTAGE == LCD_VOLTAGE_5V)
        lcd_command(0x50|c1); // power/ICON/contrast 0b0101<Ion><Bon><C5><C4>
                              //                     0b0101 0    0    X   X  =0x5X
    #elif (LCD_VOLTAGE == LCD_VOLTAGE_3V)
        lcd_command(0x54|c1); // power/ICON/contrast 0b0101<Ion><Bon><C5><C4>
                              //                     0b0101 0    1    X   X   =0x5X
    #endif
    lcd_command(0x70|(c & 0x0F));  // contrast set        0b0111<C3><C2><C1><C0>
                          //                     0b0111 X   X   X   X    =0x7X

    lcd_command(0x38); // switch back to instruction table 0  0b00111000  =0x38
#endif
}

// Sendet den Befehl zur Löschung des Displays
void lcd_clear(void) {
    lcd_command(LCD_INSTRUCTION_CLEAR);
    _delay_us(LCD_LONGDELAY);
}

// Sendet den Befehl: Cursor Home

void lcd_home(void) {
    lcd_command(LCD_INSTRUCTION_HOME);
    _delay_us(LCD_LONGDELAY);
}

// setzt den Cursor in Zeile y (1..4) Spalte x (0..15)

void lcd_cursor(uint8_t x, uint8_t y) {
    uint8_t tmp;

    switch (y) {
    case 1:
        tmp=LCD_INSTRUCTION_GOTO+LCD_BASEADRESS_LINE1+x;
        break;    // 1. Zeile
    case 2:
        tmp=LCD_INSTRUCTION_GOTO+LCD_BASEADRESS_LINE2+x;
        break;    // 2. Zeile
    case 3:
        tmp=LCD_INSTRUCTION_GOTO+LCD_BASEADRESS_LINE3+x;
        break;    // 3. Zeile
    case 4:
        tmp=LCD_INSTRUCTION_GOTO+LCD_BASEADRESS_LINE4+x;
        break;    // 4. Zeile
    default:
        return;                   // für den Fall einer falschen Zeile
    }
    lcd_command(tmp);
    _delay_us(LCD_LONGDELAY);
}

// Schreibt einen String auf das LCD

void lcd_string(char *data) {
    while(*data) {
        lcd_putc(*data);
        data++;
    }
}


int lcd_write(char data, FILE *stream) {
    lcd_putc(data);
    return 0;                // 0 zurückgeben (erfolgreiches versenden)
}

void lcd_putc(char data) {
    if (data=='\f') {
        lcd_cursor(0,1);
    } else if (data=='\n') {
        lcd_cursor(0,2);
    } else if (data=='\v') {
        lcd_cursor(0,3);
    } else if (data=='\r') {
        lcd_cursor(0,4);
    } else if (data=='°') {
        lcd_data(0xDF);
    } else if (data=='\x01') {
        lcd_data(0x00);
    } else if (data=='\x02') {
        lcd_data(0x01);
    } else if (data=='\x03') {
        lcd_data(0x02);
    } else if (data=='\x04') {
        lcd_data(0x03);
    } else if (data=='\x05') {
        lcd_data(0x04);
    } else if (data=='\x06') {
        lcd_data(0x05);
    } else if (data=='\x07') {
        lcd_data(0x06);
    } else if (data=='\x08') {
        lcd_data(0x07);
    } else if (data=='%') {
        lcd_data(0x25);
    } else if (data=='|') {
        lcd_data(0x7C);
    } else if (data=='µ') {
        lcd_data(0xE4);
    } else if (data=='@') {
        lcd_data(0x40);
    } else if (data=='ß') {
        lcd_data(0xE2);
    } else if (data=='€') {
        lcd_data(0xE3);
    } else if (data=='ä' || data=='Ä') {
        lcd_data(0xE1);
    } else if (data=='ö' || data=='Ö') {
        lcd_data(0xEF);
    } else if (data=='ü' || data=='Ü') {
        lcd_data(0xF5);
    } else if (data=='\\') {
        lcd_data(0b01011100);
    } else {
        lcd_data(data);
    }
}


void lcd_define_letter_P(uint8_t id, PGM_VOID_P data) {
    uint8_t d[8];
    memcpy_P(d, data, 8);
    lcd_define_letter(id, d);
}


void lcd_define_letter(uint8_t id, uint8_t* data) {
    unsigned char n;
    // set CGRAM adress
    uint8_t instr=0b01000000 | (((id-1)*8)&0b00111111);
    lcd_command(instr);
    for(n=0; n<8; n++) {
        lcd_data(data[n]);
    }
    // leave CGRAM, by setting moving cursor home
    lcd_home();
}
