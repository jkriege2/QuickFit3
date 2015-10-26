#ifndef OUTPUT_DIVERS_C_INCLUDED
#define OUTPUT_DIVERS_C_INCLUDED

#include <stdint.h>

// set current in mA
void setCurrent(int32_t current);

// read current in mA
int32_t getCurrent(void);

// set output parameters
void setOutputParameters(uint16_t MAXIP, uint16_t MAXIN, uint16_t MAXV);

// configure I/O pins
void configIOPins(void);


//switch TEC DC supply and Max1968 TEC controller on/off
void heating_on(void);
void heating_off(void);

// deselect any device on SPI bus!
void heating_deselect_devices(void);

// this function has to be called several times per second and coul e.g. be used to implement secondary PID regulators ...
void heating_interrupt(int32_t current);
#endif // OUTPUT_DIVERS_C_INCLUDED
