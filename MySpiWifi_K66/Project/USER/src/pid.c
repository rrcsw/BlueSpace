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

//���ֱ��ͣ�����޷�
int calcPID_W(PID *pid,float sensor_val,float sensor_grad)//λ��ʽpid
{
  float u = 0.0;
  float error = 0.0;//��ǰ�����
  float d_error = 0.0;//΢�������
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

//����޷�
int16 calcPID_Z(PID *pid,float sensor_val)//����ʽpid����������������������
{
  float delta_u = 0.0;
  float p_error = 0.0;//���������
  float i_error = 0.0;//���������

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
