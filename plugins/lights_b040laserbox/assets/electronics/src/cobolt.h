//
// LaserBox - a µController controller for Cobolt DPSS Lasers
//  (C) 2011-2013 Jan W. Krieger <j.krieger@dkfz.de, jan@jkrieger.de>
//
// This file is part of LaserBox.
//
// LaserBox is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaserBox is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Copyright Header. If not, see <http://www.gnu.org/licenses/>.
//


#  define BAUDRATE_LASER 115200
#  define LASER_DELAY_MS 10



void laser_ini(uint8_t* ERROR, uint16_t* MAX_POWER, double* WAVELENGTH) {
}


uint8_t laser_recv(char* answer, int answermax) {
    if (!laser_connected()) {
        ledpow_off();
        ledlaser_off();
        ledlock_off();
        lederror_on();
#ifdef TWO_LASERS
        if (SELECTED_LASER==0) {
            ERROR=6;
            LASER_ON=0;
        } else {
            ERROR2=6;
            LASER_ON2=0;
        }
#else
        ERROR=6;
        LASER_ON=0;
#endif
        return 0;
    }

    uint32_t t0=time_counter+10;
	uint32_t t=time_counter;
    int i=0;
    char c=-1;
    answer[0]=0;
    while (t<t0 && c!='\r' && i<answermax) {
        c=uart_getc_nowait();
        if (c>0) {
            answer[i]=c;
            i++;
        }
		t=time_counter;
    }
	answer[i]=0;

#ifdef ENABLEDEBUGMODE
	if (DEBUG_MODE!=0) {
		fprintf_P(UART, PSTR("D> Laser answers: %s\n"), answer);
#  ifndef LIGHTWEIGHTSEBUGMODE
		fprintf_P(UART, PSTR("D>         = int: %d\n"), atoi(answer));
		fprintf_P(UART, PSTR("D>         = float: %f\n"), atof(answer));
		fprintf_P(UART, PSTR("D>     t0 = %ld\n"), (long int)t0);
		fprintf_P(UART, PSTR("D>     t  = %ld\n\n"), (long int)t);
#  endif
	}
#endif
    if (t>=t0) return 0;
    return 1;
}


void laser_stat(uint8_t* ERROR, double* POW_MEAS, double* CUR_MEAS, uint8_t set_leds) {
    char answer[30];
	if (*ERROR==2) *ERROR=0;
    if (!laser_connected()) {
        *ERROR=6;
        if (set_leds) {
            ledpow_off();
            ledlaser_off();
            ledlock_off();
            lederror_on();
        }
#ifdef ENABLEDEBUGMODE
		if (DEBUG_MODE!=0) {
			uart1_puts_P(PSTR("D> laser not connected\n\n"));
		}
#endif
        return;
    }

    uart_flush();
    uart_puts("pa?\r");
	_delay_ms(LASER_DELAY_MS);
#ifdef ENABLEDEBUGMODE
	if (DEBUG_MODE!=0) {
		uart1_puts_P(PSTR("D> sent pa?\n"));
	}
#endif
    if (laser_recv(answer, 30)!=0) {
        *POW_MEAS=atof(answer)*1000.0;
    } else {
        *ERROR=2; // timeout error
    }
    if (*ERROR!=0) return;

    uart_flush();
    uart_puts("i?\r");
	_delay_ms(LASER_DELAY_MS);
#ifdef ENABLEDEBUGMODE
	if (DEBUG_MODE!=0) {
		uart1_puts_P(PSTR("D> sent i?\n"));
	}
#endif
    if (laser_recv(answer, 30)!=0) {
        *CUR_MEAS=atof(answer);
    } else {
        *ERROR=2; // timeout error
    }
    if (*ERROR!=0) return;

    uart_flush();
    uart_puts("f?\r");
	_delay_ms(LASER_DELAY_MS);
#ifdef ENABLEDEBUGMODE
	if (DEBUG_MODE!=0) {
		uart1_puts_P(PSTR("D> sent f?\n"));
	}
#endif
    if (laser_recv(answer, 30)!=0) {
		*ERROR=0;
        if (answer[0]=='1') *ERROR=3;
        if (answer[0]=='2') *ERROR=4;
        if (answer[0]=='3') *ERROR=5;
    } else {
        *ERROR=2; // timeout error
    }
    if (*ERROR!=0) return;

    if (set_leds!=0) {
        uart_flush();
        uart_puts("leds?\r");
        _delay_ms(LASER_DELAY_MS);
#ifdef ENABLEDEBUGMODE
        if (DEBUG_MODE!=0) {
            uart1_puts_P(PSTR("D> sent leds?\n"));
        }
#endif
        if (laser_recv(answer, 30)!=0) {
            uint16_t v=atoi(answer);
            if ((v & 1)==1) ledpow_on() else ledpow_off();
            //if ((v & 2)==2) ledlaser_on() else ledlaser_off();
            if ((v & 4)==4) ledlock_on() else ledlock_off();
            if ((v & 8)==8) lederror_on() else lederror_off();
        } else {
            *ERROR=2; // timeout error
        }
        if (*ERROR!=0) return;
    }
}

void laser_send_pow(uint8_t* ERROR, uint8_t LASER_ON, uint16_t POWER, uint8_t set_leds, double* POW_MEAS) {
    if (!laser_connected()) {
        *ERROR=6;
        return;
    }
    char answer[30];
    uart_flush();
    if (LASER_ON!=0) {
#ifdef ENABLEDEBUGMODE
		if (DEBUG_MODE!=0) {
			uart1_puts_P(PSTR("D> sent "));
			fprintf(UART, "p %f\n", (float)POWER/10000.0);
		}
#endif
        fprintf(UART_LASER, "p %f\r", (float)POWER/10000.0);
		if (set_leds) ledlaser_on();
		_delay_ms(LASER_DELAY_MS);
    } else {
#ifdef ENABLEDEBUGMODE
		if (DEBUG_MODE!=0) {
			uart1_puts_P(PSTR("D> sent p 0.0\n"));
		}
#endif
        fprintf(UART_LASER, "p 0.0\r");
		if (set_leds) ledlaser_off();
		_delay_ms(LASER_DELAY_MS);
	}
    if (laser_recv(answer, 30)==0) {
		*ERROR=0;
        if ((answer[0]!='O') || (answer[1]!='K')) *ERROR=1; // parameter error
    } else {
        *ERROR=2; // timeout error
    }
}


const char* errormessages[6] = {
  //----------------
   "                "
   "BAD INSTR. ERR  ",
   "LASER TIMEOUT   ",
   " TEMP. ERROR    ",
   "INTERLOCK OPEN  ",
   "CONST POW.FAULT ",
   "LASER DOSCONNEC "
};

inline char* laser_errordescription(uint8_t ERROR) {
    if (ERROR<7) return errormessages[ERROR-1];
    return errormessages[0];
}





char las_addinfo[17];
char* laser_getadditionalinfo(void) {
    int i=0;
    for (i=0; i<17; i++) {
        las_addinfo[i]=0;
    }
    sprintf_P(las_addinfo, PSTR("I=%5.3f  "), CUR_MEAS);
}
