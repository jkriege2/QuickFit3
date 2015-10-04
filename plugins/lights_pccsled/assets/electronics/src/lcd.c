#include "lcd.h"



void lcd_data(unsigned char temp1) {
    unsigned char temp2 = temp1;

#ifdef LCD_RW
    LCD_PORT &= ~(1<<LCD_RW);        // set RW=0
#endif
    LCD_PORT |= (1<<LCD_RS);        // set RS=1

    temp1 = temp1 >> 4;
    temp1 = temp1 & 0x0F;
    if (LCD_DATASTART==0) {
        LCD_PORT &= 0xF0;
        LCD_PORT |= temp1;               // setzen
    } else {
        LCD_PORT &= 0x0F;
        LCD_PORT |= (temp1<<4);               // setzen
    }
    lcd_enable();

    temp2 = temp2 & 0x0F;
    if (LCD_DATASTART==0) {
        LCD_PORT &= 0xF0;
        LCD_PORT |= temp2;               // setzen
    } else {
        LCD_PORT &= 0x0F;
        LCD_PORT |= (temp2<<4);               // setzen
    }
    lcd_enable();

    _delay_us(LCD_SHORTDELAY);
}

// sendet einen Befehl an das LCD

void lcd_command(unsigned char temp1) {
    unsigned char temp2 = temp1;

#ifdef LCD_RW
    LCD_PORT &= ~(1<<LCD_RW);        // set RW=0
#endif
    LCD_PORT &= ~(1<<LCD_RS);        // RS auf 0 setzen

    temp1 = temp1 >> 4;              // oberes Nibble holen
    temp1 = temp1 & 0x0F;            // maskieren
    if (LCD_DATASTART==0) {
        LCD_PORT &= 0xF0;
        LCD_PORT |= temp1;               // setzen
    } else {
        LCD_PORT &= 0x0F;
        LCD_PORT |= (temp1<<4);               // setzen
    }
    lcd_enable();

    temp2 = temp2 & 0x0F;            // unteres Nibble holen und maskieren
    if (LCD_DATASTART==0) {
        LCD_PORT &= 0xF0;
        LCD_PORT |= temp2;               // setzen
    } else {
        LCD_PORT &= 0x0F;
        LCD_PORT |= (temp2<<4);               // setzen
    }
    lcd_enable();

    _delay_us(LCD_SHORTDELAY);
}

// erzeugt den Enable-Puls
void lcd_enable(void) {
    // Bei Problemen ggf. Pause gemäß Datenblatt des LCD Controllers einfügen
    // http://www.mikrocontroller.net/topic/81974#685882
    LCD_PORT |= (1<<LCD_EN);
    _delay_us(10);                   // kurze Pause
    // Bei Problemen ggf. Pause gemäß Datenblatt des LCD Controllers verlängern
    // http://www.mikrocontroller.net/topic/80900
    LCD_PORT &= ~(1<<LCD_EN);
}

// Initialisierung:
// Muss ganz am Anfang des Programms aufgerufen werden.

void lcd_init(void) {
#ifdef LCD_RW
    LCD_DDR = LCD_DDR | 0xF0 | (1<<LCD_RS) | (1<<LCD_RW) | (1<<LCD_EN);   // Port auf Ausgang schalten
#else
    LCD_DDR = LCD_DDR | 0xF0 | (1<<LCD_RS) | (1<<LCD_EN);   // Port auf Ausgang schalten
#endif
    _delay_ms(20);

    // muss 3mal hintereinander gesendet werden zur Initialisierung

#ifdef LCD_RW
    LCD_PORT &= ~(1<<LCD_RW);        // set RW=0
#endif
    _delay_ms(20);

    if (LCD_DATASTART==0) {
        LCD_PORT &= 0xF0;
        LCD_PORT |= 0x03;               // setzen
    } else {
        LCD_PORT &= 0x0F;
        LCD_PORT |= (0x03<<4);               // setzen
    }

    LCD_PORT &= ~(1<<LCD_RS);      // RS auf 0
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
}

// Sendet den Befehl zur Löschung des Displays

void lcd_clear(void) {
    lcd_command(CLEAR_DISPLAY);
    _delay_us(LCD_LONGDELAY);
}

// Sendet den Befehl: Cursor Home

void lcd_home(void) {
    lcd_command(CURSOR_HOME);
    _delay_us(LCD_LONGDELAY);
}

// setzt den Cursor in Zeile y (1..4) Spalte x (0..15)

void lcd_cursor(uint8_t x, uint8_t y) {
    uint8_t tmp;

    switch (y) {
    case 1:
        tmp=0x80+0x00+x;
        break;    // 1. Zeile
    case 2:
        tmp=0x80+0x40+x;
        break;    // 2. Zeile
    case 3:
        tmp=0x80+0x10+x;
        break;    // 3. Zeile
    case 4:
        tmp=0x80+0x50+x;
        break;    // 4. Zeile
    default:
        return;                   // für den Fall einer falschen Zeile
    }
    lcd_command(tmp);
}

// Schreibt einen String auf das LCD

void lcd_string(char *data) {
    while(*data) {
        if (*data=='°') {
            lcd_data(0xDF);
        } else if (*data=='\x01') {
            lcd_data(0x00);
        } else if (*data=='\x02') {
            lcd_data(0x01);
        } else if (*data=='\x03') {
            lcd_data(0x02);
        } else if (*data=='\x04') {
            lcd_data(0x03);
        } else if (*data=='\x05') {
            lcd_data(0x04);
        } else if (*data=='\x06') {
            lcd_data(0x05);
        } else if (*data=='\x07') {
            lcd_data(0x06);
        } else if (*data=='\x08') {
            lcd_data(0x07);
            /*} else if (*data=='\\') {
            lcd_data(0b01011100);*/
        } else {
            lcd_data(*data);
        }
        data++;
    }
}


int lcd_write(char data, FILE *stream) {
    if (data=='\f') {
        lcd_cursor(0,1);
    } else if (data=='\n') {
        lcd_cursor(0,2);
    } else if (data=='\b') {
        lcd_cursor(0,3);
    } else if (data=='\r') {
        lcd_cursor(0,4);
    } else if (data=='°') {
        lcd_data(0xDF);
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
    return 0;                // 0 zurückgeben (erfolgreiches versenden)
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
