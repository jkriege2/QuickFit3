#define TIMER_IMPLEMENT_VARIABLES
#include "clocktimer.h"


// interrupt service routine for timer/counter1 compare A interrupt
ISR(TIMER1_COMPA_vect) {
    timer+=10;
    timeout_timer+=10;
}

