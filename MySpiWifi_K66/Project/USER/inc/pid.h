#ifndef _pid_h
#define _pid_h

#include "headfile.h"
#include "math.h"

void initPID(PID *pid);
void changeSetPoint(PID *pid,float set_point);
void setPIDParam(PID *pid,float P,float I,float D);
int calcPID_W(PID *pid,float sensor_val,float sensor_grad);
int16 calcPID_Z(PID *pid,float sensor_val);

#endif
