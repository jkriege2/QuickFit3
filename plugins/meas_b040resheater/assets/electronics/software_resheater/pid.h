#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED

#include <stdint.h>

typedef struct {
    int16_t P;
    int16_t I;
    int16_t D;
    int32_t I_MAX;
    int32_t integration_error;
} PID_PARAMS;

extern int32_t pid_control(PID_PARAMS* pid, int16_t targetvalue, int16_t actualvalue);

extern void pid_set(PID_PARAMS* pid, int16_t P, int16_t D, int16_t I, int32_t I_MAX);

#endif // PID_H_INCLUDED
