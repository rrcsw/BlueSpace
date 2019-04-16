#ifndef _decision_h
#define _decision_h

#include "headfile.h"

void Decision(PID *pid_steer, PID *pid_left_motor, PID *pid_right_motor);


void DifferentialSpeed(PID *left_pid_v,PID *right_pid_v);
void Dynamic_P(PID *pid,float sensor_val);

#endif