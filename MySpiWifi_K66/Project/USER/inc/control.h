#ifndef _control_h
#define _control_h

#include "headfile.h"
#include <math.h>

void Control(PID *pid_steer, PID *pid_left_motor, PID *pid_right_motor);

void SpeedControl(PID *pid_left_motor,PID *pid_right_motor);
void AngleControl(PID *pid_steer);

void setPWMDuty(int16 left_motor_u,int16 right_motor_u);
void setPWMWidth(uint16 steer_u);

int16 getLeftMotor_u(void);
int16 getRightMotor_u(void);
void setLeftMotor_u(int16 value);
void setRightMotor_u(int16 value);
uint16 getSteer_u(void);
void setSteer_u(uint16 value);

//反向制动
int16_t Bangbang(PID *pid,float sensor_val);

#endif
