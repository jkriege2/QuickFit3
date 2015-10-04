#include "pid.h"

//PI temperature control
int32_t pid_control(PID_PARAMS* pid, int16_t targetvalue, int16_t actualvalue)
{
	//int32_t new_output=0;

	int32_t err_0 = targetvalue - actualvalue;	//current error
	pid->integration_error += err_0;						//Integration part

    const int16_t comp=pid->I_MAX;///pid->I;

	if (pid->integration_error > comp) pid->integration_error = comp;	//variable limitation: avoid huge integration and thus heating too long
	if (pid->integration_error < -comp) pid->integration_error = -comp;

	//return new_output;
	return (pid->P*err_0 + pid->I*pid->integration_error);			//PI regulation
}


void pid_set(PID_PARAMS* pid, int16_t P, int16_t D, int16_t I, int32_t I_MAX) {
    pid->I=I;
    pid->P=P;
    pid->D=D;
    pid->integration_error=0;
    pid->I_MAX=I_MAX;
}
