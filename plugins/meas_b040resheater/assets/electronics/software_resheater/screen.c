#include "screen.h"
#include "lcd.h"
#include "tools.h"


void lcd_basic_screen(FILE* lcd) {
    lcd_string_line1(" T0=     °C Ti=   °C");
    lcd_string_line2(" T1=     °C         ");
    lcd_string_line3("set=     °C  O[    ]");
    lcd_string_line4("Menu   \xE0    \xDE  OnOff");
}

void lcd_display_PWM_off(FILE* lcd) {
    lcd_pos_string(13, 3, "       ");
}

void lcd_display_PWM(FILE* lcd, int16_t out, int16_t outmin, int16_t out_max) {
    char t[10]={'O','[',0b11000100,0b11000100,0b11000100,0b11000100,']',0,0,0};
    //5*6=30;
    int32_t val=(int32_t)(out-outmin)*(int32_t)25;
    val=val/(out_max-outmin);
    int full=val/6;
    int half=val%6;
    if (full>4) full=5;
    for (char i=0; i<full; i++) t[i+2]=0b11010000;
    //t[full+2]=0b11000100;
    switch(half) {
        case 5: t[full+2]=0b11010000; break;
        case 4: t[full+2]=0b11010001; break;
        case 3: t[full+2]=0b11010010; break;
        case 2: t[full+2]=0b11010011; break;
        case 1: t[full+2]=0b11010100; break;
        default:
        case 0: t[full+2]=0b11000100; break;
    }
    t[6]=']';
    t[7]=0;
    lcd_pos_string(13, 3, t);
}

void lcd_display_temp(FILE* lcd, int8_t source, int16_t temp0, int16_t temp1, int16_t int_temp, int16_t set_temp) {
    char t[10];
    if ((temp0>=10000)||(temp0<=-10000)) {
        lcd_pos_string(4, 1, " --- ");
    } else {
        sprintf(t, "%4i", temp0);
        t[5]=0;
        t[4]=t[3];
        t[3]='.';
        lcd_pos_string(4, 1, t);
    }

    if ((temp1>=10000)||(temp1<=-10000)) {
        lcd_pos_string(4, 2, " --- ");
    } else {
        sprintf(t, "%4i", temp1);
        to_decimal(t, 1);
        lcd_pos_string(4, 2, t);
    }

    if ((set_temp>=10000)||(set_temp<=-10000)) {
        lcd_pos_string(4, 2, " --- ");
    } else {
        sprintf(t, "%4i", set_temp);
        to_decimal(t, 1);
        lcd_pos_string(4, 3, t);
    }

    sprintf(t, "%3i", int_temp);
    lcd_pos_string(15, 1, t);
    if (source==0) {
        lcd_pos_char(0,1,0b00010000);
        lcd_pos_char(0,2,' ');
    } else if (source==1) {
        lcd_pos_char(0,1,' ');
        lcd_pos_char(0,2,0b00010000);
    } else if (source==2) {
        lcd_pos_char(0,1,0b00010000);
        lcd_pos_char(0,2,0b00010000);
    } else {
        lcd_pos_char(0,1,' ');
        lcd_pos_char(0,2,' ');
    }

}

/*
//prints just the measured values (current, temperature 0,1,int) when not in menu
void lcd_update_measured(FILE* lcd, int16_t temp0, int16_t temp1, int16_t int_temp, int16_t act_current, int16_t SET_CURRENT, uint8_t CONTROL_TYPE, uint8_t heating_state, uint8_t sensor_status) {
    if ((sensor_status&0x01)!=0) {
        lcd_gotoxy(4,0);
        fprintf(lcd,"%5i",(int16_t)temp0);
        lcd_gotoxy(4,0);
        fprintf(lcd,"%3i",(int16_t)temp0/10);
        fprintf(lcd,".");
    } else {
        lcd_gotoxy(4,0);
        fprintf(lcd,"  ---");
    }


    if ((sensor_status&0x02)!=0) {
        lcd_gotoxy(4,1);
        fprintf(lcd,"%5i",(int16_t)temp1);
        lcd_gotoxy(4,1);
        fprintf(lcd,"%3i",(int16_t)temp1/10);
        fprintf(lcd,".");
    } else {
        lcd_gotoxy(4,1);
        fprintf(lcd,"  ---");
    }

    lcd_gotoxy(15,0);
    fprintf(lcd,"%3i",int_temp);


    if (CONTROL_TYPE) {
        if (heating_state!=0) {
            lcd_gotoxy(15,2);
            fprintf(lcd,"%5i",(int16_t)act_current);
            //fprintf(lcd,"%5i",(int16_t)SET_CURRENT);
        } else {
            lcd_gotoxy(15,2);
            fprintf(lcd,"%5i",0);
        }

    } else {

        if (heating_state!=0) {
            lcd_gotoxy(15,2);
            //fprintf(lcd,"%5i",(int16_t)act_current);
            fprintf(lcd,"%5i",(int16_t)SET_CURRENT);
        } else {
            lcd_gotoxy(15,2);
            fprintf(lcd,"%5i",0);
        }

    }
}

*/

/*
//prints just values set by user (TARGET_TEMPERATUREerature, SET_CURRENT) when not in menu
void lcd_update_set(FILE* lcd, int16_t SET_CURRENT, int16_t TARGET_TEMPERATURE, uint8_t CONTROL_TYPE) {

    lcd_gotoxy(4,2);
    if (CONTROL_TYPE) {
        fprintf(lcd,"%5i",(int16_t)SET_CURRENT);
    } else {
        fprintf(lcd,"%5i",(int16_t)TARGET_TEMPERATURE);
        lcd_gotoxy(4,2);
        fprintf(lcd,"%3i",(int16_t)TARGET_TEMPERATURE/10);
        fprintf(lcd,".");
    }
}


void lcd_normal_screen(FILE* lcd, int16_t temp0, int16_t temp1, int16_t int_temp, int16_t TARGET_TEMPERATURE, int16_t act_current, int16_t SET_CURRENT, uint8_t CONTROL_TYPE, uint8_t heating_state, uint8_t TEMPERATURE_SOURCE) {
    fprintf(lcd," T0 %5i°C In %3i°C",(int16_t)temp0,int_temp);
    lcd_gotoxy(4,0);
    //print temperature again to display 1/10 °C nicely
    fprintf(lcd,"%3i",(int16_t)temp0/10);
    fprintf(lcd,".");

    lcd_gotoxy(0,1);
    if (CONTROL_TYPE) {
        fprintf(lcd," T1 %5i°C  I[mA] ~",(int16_t)temp1);
        lcd_gotoxy(4,1);
        fprintf(lcd,"%3i",(int16_t)temp1/10);
        fprintf(lcd,".");

        lcd_gotoxy(0,2);

        if (heating_state!=0) {
            fprintf(lcd,"set %5i  act %5i",SET_CURRENT,act_current);
        } else {
            fprintf(lcd,"set %5i  act   ---",SET_CURRENT);
        }
        lcd_gotoxy(0,3);
        fprintf(lcd,"Menu   ~    #  OnOff");
    } else {
        fprintf(lcd," T1 %5i°C  I[mA] ~",(int16_t)temp1);
        lcd_gotoxy(4,1);
        fprintf(lcd,"%3i",(int16_t)temp1/10);
        fprintf(lcd,".");

        lcd_gotoxy(0,2);
        if (heating_state!=0) {
            fprintf(lcd,"set %5i°C    %5i",(int16_t)TARGET_TEMPERATURE,(int16_t)act_current);
            lcd_gotoxy(4,2);
            fprintf(lcd,"%3i",(int16_t)TARGET_TEMPERATURE/10);
            fprintf(lcd,".");
        } else {
            fprintf(lcd,"set %5i°C      ---",(int16_t)TARGET_TEMPERATURE);
            lcd_gotoxy(4,2);
            fprintf(lcd,"%3i",(int16_t)TARGET_TEMPERATURE/10);
            fprintf(lcd,".");
        }
        switch (TEMPERATURE_SOURCE){		//mark which source is used for temp control
            case 0: lcd_gotoxy(0,0); fprintf(lcd,"{"); break;
            case 1: lcd_gotoxy(0,1); fprintf(lcd,"{"); break;
            case 2: lcd_gotoxy(0,0); fprintf(lcd,"{"); lcd_gotoxy(0,1); fprintf(lcd,"{"); break;
            case 3: lcd_gotoxy(11,0); fprintf(lcd,"{"); break;
        }
        lcd_gotoxy(0,3);
        fprintf(lcd,"Menu   ~    #  OnOff");
    }
}
*/
