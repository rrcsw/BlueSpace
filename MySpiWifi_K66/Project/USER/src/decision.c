#include "decision.h"


//float Dynamic_P_Denominator = 100.0;
//float Dynamic_P_Constant = 4.5;//0.6;

//float Dynamic_P_Denominator = 100.0;
//float Dynamic_P_Constant = 3.5;//0.6;

float Dynamic_P_Denominator = 150.0;
float Dynamic_P_Constant = 4.1;//3.9;//5;//3.5;//0.6;

//float Dynamic_P_Denominator = 2000.0;
//float Dynamic_P_Constant = 6;

float DiffSpeedTheta = 0.7853;
float DiffSpeed_P = 0.3;

void Decision(PID *pid_steer, PID *pid_left_motor, PID *pid_right_motor)
{
  Dynamic_P(pid_steer, getMiddleLine());//��̬����P
  //SpeedPlanning();//�ٶȹ滮
  //DifferentialSpeed(PID *left_pid_v,PID *right_pid_v);//����
}

void CustomRule()
{
  
}

void SpeedPlanning(PID *pid_angle, PID *left_pid_v, PID *right_pid_v)
{
//  int max_speed = 350;//
//  int min_speed = 220;
//  int left_speed, right_speed;
//  left_speed = right_speed = 0;
  
  
  //������٣�������٣��������������㣬 ��ֱ���ͼ��٣���ֱ�������٣�
  //���£� �ᳵ��  Բ�����٣� ��«����٣�Сs �ͼ��٣�бʮ�ּ��٣� 
  //ͣ�����ƣ����ϼ��٣����ٶȼǸ������ϰ���
  
}
/*
void DifferentialSpeed(PID *left_pid_v,PID *right_pid_v)
{
  float v_left = 0.0;
  float v_right = 0.0;
//����angle��Ҳ�У�����lastline��Ҳ��
  float error = getMiddleLine() - (MID_LINE_VAL);
  float theta = (error / (IMAGE_WIDTH / 2)) * DiffSpeedTheta;
  DiffSpeed_P = (float) (30 / ((IMAGE_HEIGHT - (getLastLine() + 4)) * (IMAGE_HEIGHT - (getLastLine() + 4))));
  float k = DiffSpeed_P * tan(theta);
if(error > 10)//ƫ�����ĳ��ֵ�ſ�ʼ����
#if DIR == 0
  v_left = average_speed * (1 + k);
  v_right = average_speed * (1 - k);
#elif DIR == 1
  v_left = average_speed * (1 - k);
  v_right = average_speed * (1 + k);

if(v_left > 350)
  v_left = 350;
if(v_left <= 0)
  v_left = 0;
if(v_right > 350)
  v_right = 350;
if(v_right <= 0)
  v_right = 0;

  changeSetPoint(left_pid_v,v_left);
  changeSetPoint(right_pid_v,v_right);
}*/

void Dynamic_P(PID *pid,float sensor_val)
{
  float bias_square = (pid->set_point - sensor_val) * (pid->set_point - sensor_val);//ƫ���ƽ��
  pid->K_p = bias_square / Dynamic_P_Denominator + Dynamic_P_Constant;
  pid->K_p = pid->K_p > 15 ? 15 : pid->K_p;//��̬����p�޷�
}


//�����������ͣ�������p�Ĵ�С
//�������ļ����Σ�����ʲô��
void PID_Planning()
{
  
}

void PathPlanning()
{
  
}