#define GLOBALDEFINITIONS
#include "config.h"
#include "lcd.h"

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

#define READFROMEEPROM(param, adr, size) \
  eeprom_read_block((void*)&param, (void*)adr, size)




void configSave(void) {
    configCalc();
    eeprom_write_word((void*)ADR_FIRSTTIME, 0xEEEE);

    SAVETOEEPROM(PT100_OFFSET0, ADR_PT100_OFFSET0, 2);
    SAVETOEEPROM(PT100_OFFSET1, ADR_PT100_OFFSET1, 2);
    SAVETOEEPROM(SET_TEMP, ADR_SET_TEMP, 2);
    SAVETOEEPROM(BAUDRATE, ADR_BAUDRATE, 4);
    SAVETOEEPROM(T_INPUT, ADR_T_INPUT, 1);
    SAVETOEEPROM(FAN_INCREASE, ADR_FAN_INCREASE, 2);
    SAVETOEEPROM(SHUTDOWN, ADR_SHUTDOWN, 2);
    SAVETOEEPROM(FAN_MIN, ADR_FAN_MIN, 2);
    SAVETOEEPROM(PID_I, ADR_PID_I, 2);
    SAVETOEEPROM(PID_P, ADR_PID_P, 2);
    SAVETOEEPROM(PID_D, ADR_PID_D, 2);
    SAVETOEEPROM(PID_IMAX, ADR_PID_IMAX, 4);


}

void configLoad(void) {
    if (eeprom_read_word((void*)ADR_FIRSTTIME)!=0xEEEE) {
        configLoadStd();
    } else {
        READFROMEEPROM(SET_TEMP, ADR_SET_TEMP, 2);
        READFROMEEPROM(PT100_OFFSET0, ADR_PT100_OFFSET0, 2);
        READFROMEEPROM(PT100_OFFSET1, ADR_PT100_OFFSET1, 2);
        READFROMEEPROM(T_INPUT, ADR_T_INPUT, 1);
        READFROMEEPROM(BAUDRATE, ADR_BAUDRATE, 4);
        READFROMEEPROM(FAN_INCREASE, ADR_FAN_INCREASE, 2);
        READFROMEEPROM(SHUTDOWN, ADR_SHUTDOWN, 2);
        READFROMEEPROM(FAN_MIN, ADR_FAN_MIN, 2);
        READFROMEEPROM(PID_I, ADR_PID_I, 2);
        READFROMEEPROM(PID_P, ADR_PID_P, 2);
        READFROMEEPROM(PID_D, ADR_PID_D, 2);
        READFROMEEPROM(PID_IMAX, ADR_PID_IMAX, 4);
    }
    configCalc();
}


void configLoadStd(void) {
    BAUDRATE=115200;
    SET_TEMP=370;
    T_INPUT=0;
    PT100_OFFSET1=25;
    PT100_OFFSET0=25;
    SHUTDOWN=70;
    FAN_MIN=30;
    FAN_INCREASE=3;
    PID_P=20;
    PID_I=1;
    PID_D=0;
    PID_IMAX=100;
    configCalc();
    configSave();
}


void configCalc(void) {
}

void configSaveNonMenu(void) {
    SAVETOEEPROM(SET_TEMP, ADR_SET_TEMP, 2);
}
