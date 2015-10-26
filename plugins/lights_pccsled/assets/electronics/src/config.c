#define GLOBALDEFINITIONS
#include "config.h"
#include "leds.h"

#define SAVETOEEPROM(param, adr, size) \
  { \
     uint8_t temp[size]; \
     uint8_t* d=(uint8_t*)&(param); \
     char write=0; \
     int i; \
     eeprom_read_block((void*)temp, (void*)(adr), size); \
     for (i=0; i<size; i++) { \
         if (temp[i]!=d[i]) { write =1; break; } \
     } \
     if (write==1) { \
         eeprom_write_block((void*)&(param), (void*)(adr), size); \
     } \
  } \

void configSave(void) {
    configCalc();
    eeprom_write_word((void*)ADR_FIRSTTIME, 0xEEEE);

    SAVETOEEPROM(FAN_START, ADR_FAN_START, 2);
    SAVETOEEPROM(FAN_FACTOR, ADR_FAN_FACTOR, 2);
    SAVETOEEPROM(FAN_OFFSET, ADR_FAN_OFFSET, 2);
    SAVETOEEPROM(LCD_BACKLIGHT, ADR_LCD_BACKLIGHT, 2);
    SAVETOEEPROM(CURRENT, ADR_CURRENT, 2);
    SAVETOEEPROM(BAUDRATE, ADR_BAUDRATE, 4);
    SAVETOEEPROM(CURRENT_MAX, ADR_CURRENT_MAX, 2);
    SAVETOEEPROM(CURRENT_100, ADR_CURRENT_100, 2);
    SAVETOEEPROM(LED, ADR_LED, 2);
    SAVETOEEPROM(IADC_MEASUREMENTS, ADR_IADC_MEASUREMENTS, 2);
    SAVETOEEPROM(EADC_MEASUREMENTS, ADR_EADC_MEASUREMENTS, 2);
    SAVETOEEPROM(IADC_WAIT, ADR_IADC_WAIT, 2);
    SAVETOEEPROM(EADC_WAIT, ADR_EADC_WAIT, 2);
    SAVETOEEPROM(LEDBLINK, ADR_LEDBLINK, 2);
    //SAVETOEEPROM(, ADR_, 2);
    /* eeprom_write_block((void*)&FAN_START, (void*)ADR_FAN_START, 2);
    eeprom_write_block((void*)&FAN_FACTOR, (void*)ADR_FAN_FACTOR, 2);
    eeprom_write_block((void*)&FAN_OFFSET, (void*)ADR_FAN_OFFSET, 2);
    eeprom_write_block((void*)&LCD_BACKLIGHT, (void*)ADR_LCD_BACKLIGHT, 1);
    eeprom_write_block((void*)&CURRENT, (void*)ADR_CURRENT, 2);
    eeprom_write_block((void*)&BAUDRATE, (void*)ADR_BAUDRATE, 4);
    eeprom_write_block((void*)&CURRENT_MAX, (void*)ADR_CURRENT_MAX, 2);
    eeprom_write_block((void*)&CURRENT_100, (void*)ADR_CURRENT_100, 2);
    eeprom_write_block((void*)&LED, (void*)ADR_LED, 1);
    eeprom_write_block((void*)&IADC_MEASUREMENTS, (void*)ADR_IADC_MEASUREMENTS, 2);
    eeprom_write_block((void*)&EADC_MEASUREMENTS, (void*)ADR_EADC_MEASUREMENTS, 2);
    eeprom_write_block((void*)&IADC_WAIT, (void*)ADR_IADC_WAIT, 2);
    eeprom_write_block((void*)&EADC_WAIT, (void*)ADR_EADC_WAIT, 2);
    eeprom_write_block((void*)&LEDBLINK, (void*)ADR_LEDBLINK, 2); */
}

void configLoad(void) {
    if (eeprom_read_word((void*)ADR_FIRSTTIME)!=0xEEEE) {
        configLoadStd();
    } else {
        eeprom_read_block((void*)&FAN_START, (void*)ADR_FAN_START, 2);
        eeprom_read_block((void*)&FAN_FACTOR, (void*)ADR_FAN_FACTOR, 2);
        eeprom_read_block((void*)&FAN_OFFSET, (void*)ADR_FAN_OFFSET, 2);
        eeprom_read_block((void*)&LCD_BACKLIGHT, (void*)ADR_LCD_BACKLIGHT, 1);
        eeprom_read_block((void*)&CURRENT, (void*)ADR_CURRENT, 2);
        eeprom_read_block((void*)&CURRENT_MAX, (void*)ADR_CURRENT_MAX, 2);
        eeprom_read_block((void*)&CURRENT_100, (void*)ADR_CURRENT_100, 2);
        eeprom_read_block((void*)&BAUDRATE, (void*)ADR_BAUDRATE, 4);
        eeprom_read_block((void*)&LED, (void*)ADR_LED, 1);
        eeprom_read_block((void*)&IADC_MEASUREMENTS, (void*)ADR_IADC_MEASUREMENTS, 2);
        eeprom_read_block((void*)&EADC_MEASUREMENTS, (void*)ADR_EADC_MEASUREMENTS, 2);
        eeprom_read_block((void*)&IADC_WAIT, (void*)ADR_IADC_WAIT, 2);
        eeprom_read_block((void*)&EADC_WAIT, (void*)ADR_EADC_WAIT, 2);
        eeprom_read_block((void*)&LEDBLINK, (void*)ADR_LEDBLINK, 2);
    }
    configCalc();
}


void configLoadStd(void) {
    FAN_START=36;
    FAN_FACTOR=6;
    FAN_OFFSET=120;
    LCD_BACKLIGHT=125;
    BAUDRATE=9600;
    CURRENT=0;
    CURRENT_MAX=2000;
    CURRENT_100=CURRENT_MAX;
    LED=0;
    IADC_MEASUREMENTS=32;
    IADC_WAIT=25;
    EADC_MEASUREMENTS=1;
    EADC_WAIT=0;
    LEDBLINK=0;
    configCalc();
    configSave();
}

void configCalc(void) {
    if (LED>0 && LED<led_count) {
        CURRENT_MAX=pgm_read_word(&(led_currentmax[LED]));
        CURRENT_100=pgm_read_word(&(led_current100[LED]));
    } else LED=0;
    LEDBLINK_compare=62500/(LEDBLINK*2);
}

void configSaveNonMenu(void) {
    eeprom_write_block((void*)&CURRENT, (void*)ADR_CURRENT, 2);
}
