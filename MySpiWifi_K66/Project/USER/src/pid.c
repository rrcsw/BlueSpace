#include "pid.h"

void initPID(PID *pid)
{
  pid->set_point = 0.0;
  pid->K_p = 0.0;
  pid->K_i = 0.0;
  pid->K_i = 0.0;
  pid->sum_error = 0.0;
  pid->last_error = 0.0;
  pid->last_2_error = 0.0;
}

void changeSetPoint(PID *pid,float set_point)
{
  pid->set_point = set_point;
}

void setPIDParam(PID *pid,float P,float I,float D)
{
  pid->K_p = P;
  pid->K_i = I;
  pid->K_d = D;
}

//积分饱和，输出限幅
int calcPID_W(PID *pid,float sensor_val,float sensor_grad)//位置式pid
{
  float u = 0.0;
  float error = 0.0;//当前误差项
  float d_error = 0.0;//微分误差项
  error	= pid->set_point - sensor_val;
  d_error = error - pid->last_error;
  
  if(fabs(error) < 0.5)
    u = 0;
  else
    u = pid->K_p * error + pid->K_d * d_error;
  pid->last_2_error = pid->last_error;
  pid->last_error = error;
  return (int)u;
}

//输出限幅
int16 calcPID_Z(PID *pid,float sensor_val)//增量式pid，无需计算积分项，即积累误差
{
  float delta_u = 0.0;
  float p_error = 0.0;//比例误差项
  float i_error = 0.0;//积分误差项

  i_error = pid->set_point - sensor_val;
  p_error = i_error - pid->last_error;
  
  if(fabs(i_error) < 1.5)
    delta_u = 0;
  else
    delta_u = pid->K_p * p_error + pid->K_i * i_error;
  pid->last_2_error = pid->last_error;
  pid->last_error = i_error;
  return (int)delta_u;
}
