#include "control.h"

uint8 control_mode = HALF_CLOSE_LOOP;//初始控制模式

uint16 steer_u = 0;
int16 left_motor_u = 0;
int16 right_motor_u = 0;

//pid计算；周期控制为10ms
void Control(PID *pid_steer, PID *pid_left_motor, PID *pid_right_motor)
{
  switch(control_mode)
  {
  case CLOSE_LOOP://闭环
    AngleControl(pid_steer);//输出限幅，积分饱和
    SpeedControl(pid_left_motor, pid_right_motor);
    break;
  case HALF_CLOSE_LOOP://半闭环，只有舵机pid
    AngleControl(pid_steer);//输出限幅，积分饱和
    //SpeedControl(pid_left_motor, pid_right_motor);
    break;
  case OPEN_LOOP_MEETING_CAR_WAITING://开环 让出会车并等待
    //开始记录编码器的距离
    //if 编码器 < 30cm
    //    setPWMWidth(STEER_MIN);//左打死
    //    左右电机给固定PWM
    //else
    //    停下来，等待对方回应
    //     
    break;
  case OPEN_LOOP_MEETING_CAR_ING://开环 会车 回到赛道
    //停止记录编码器，并清零
    //if  还没回到 赛道中央
    //    setPWMWidth(STEER_MIN);//左打死
    //    左右电机给固定反转PWM
    //else
    //    触发会车完成事件
    //     
    break;
  default:
    break;
  }

  float v_left = 0.0;
  float v_right = 0.0;
//根据angle来也行，根据lastline来也行
  float error = getMiddleLine() - MID_LINE_VAL;
  range_protect(&error, 40, - 40, 3);
  /*float theta = (error / (IMAGE_WIDTH / 2)) * DiffSpeedTheta;
  //printf("%.2f \r \n", tan(DiffSpeedTheta));
  DiffSpeed_P = (float) (900 / ((IMAGE_HEIGHT - (getLastLine() + 4)) * (IMAGE_HEIGHT - (getLastLine() + 4))));
  float k = DiffSpeed_P * tan(theta);*/
  
  float k = 0;
  k = 0.4 * tan(error * 0.0373);
  
  k = k > 4 ? 4 : k;
  k = k < - 4 ? - 4 : k;
  
  if(error > 4)//偏差大于某个值才开始差速
  {
    v_left = left_motor_u * (1 + 0.2 * k);
    v_right = right_motor_u * (1 - 1.5 * k);
  }
  else if(error < - 4)//偏差大于某个值才开始差速
  {
    v_left = left_motor_u * (1 + 1.2 * k);
    v_right = right_motor_u * (1 - 0.2 * k);
  }
  else
  {
    v_left = left_motor_u;
    v_right = right_motor_u;
  }
  setPWMDuty((int)v_left, (int)v_right);
  //setPWMDuty(left_motor_u, right_motor_u);
}

void handleMessage_CTRL(void)
{
  while(hasMessage(CTRL_))
  {
    uint8 data[MSG_LENGTH] = { 0 };
    switch(ReadMessage(CTRL_, data))
    {
    case WM_CTRL:
      Debug("测试消息框架的控制层");
      break;
    case WM_CTRL_CLOSE_LOOP_MODE:
      control_mode = CLOSE_LOOP;
      break;
    case WM_CTRL_OPEN_LOOP_MEETING_CAR_MODE:
      
      break;
    default:
      break;
    }
  }
}

void AngleControl(PID *pid_a)
{
  float angle = getAngle();
  if(angle > IMAGE_WIDTH * 2)//中线偏差限幅
    angle = IMAGE_WIDTH * 2;
  else if(angle < - IMAGE_WIDTH * 1)
    angle = - IMAGE_WIDTH * 1;
#if DIR == 0//左小右大
  steer_u = STEER_MID - calcPID_W(pid_a, getMiddleLine(), 0);
#elif DIR == 1
  steer_u = STEER_MID + calcPID_W(pid_a, getMiddleLine(), 0);
#endif
//舵机脉宽平滑变化
  setPWMWidth(steer_u);
}

void SpeedControl(PID *left_pid_v,PID *right_pid_v)
{
  float left_speed = getLeftSpeed();
  float right_speed = getRightSpeed();
  int16_t bangbang_l = Bangbang(left_pid_v,left_speed);
  int16_t bangbang_r = Bangbang(right_pid_v,right_speed);
  int16_t pid_output_l = calcPID_Z(left_pid_v,left_speed);
  int16_t pid_output_r = calcPID_Z(right_pid_v,right_speed);

  if(bangbang_l != 0)
    left_motor_u += bangbang_l;
  else
    left_motor_u += pid_output_l;
  if(bangbang_r != 0)
    right_motor_u += bangbang_r;
  else
    right_motor_u += pid_output_r;
  setPWMDuty(left_motor_u, right_motor_u);
}

int16_t Bangbang(PID *pid,float sensor_val)//两个档位的棒棒控制
{
  float err = pid->set_point - sensor_val;
  if(err > BANGBANG_THRESH)
    return (int16_t)(BANGBANG_OUTPUT * 0.7);
  else if(err > BANGBANG_THRESH * 0.5)
    return (int16_t)(BANGBANG_OUTPUT * 0.35);
  
  else if(err < - BANGBANG_THRESH)
    return (int16_t)(- BANGBANG_OUTPUT * 0.75);
  else if(err < - BANGBANG_THRESH * 0.5)
    return (int16_t)(BANGBANG_OUTPUT * 0.4);
  return 0;
}

void setPWMWidth(uint16 steer_u)
{
  if(steer_u > STEER_MAX)
  {
    steer_u = STEER_MAX;
  }
  if(steer_u < STEER_MIN)
  {
    steer_u = STEER_MIN;
  }
  ftm_pwm_duty(ftm0,ftm_ch5,steer_u);
}

void setPWMDuty(int16 left_motor_u,int16 right_motor_u)
{
  if(left_motor_u > PWM_MAX)
    left_motor_u = PWM_MAX;
  else if(left_motor_u < - PWM_MAX)
    left_motor_u = - PWM_MAX;
  if(right_motor_u > PWM_MAX)
    right_motor_u = PWM_MAX;
  else if(right_motor_u < - PWM_MAX)
    right_motor_u = - PWM_MAX;
  
  if(left_motor_u >= 0)
  {
    ftm_pwm_duty(ftm3,ftm_ch4, abs(left_motor_u));
    ftm_pwm_duty(ftm3,ftm_ch5,0);
  }
  else
  {
    ftm_pwm_duty(ftm3,ftm_ch4,0);
    ftm_pwm_duty(ftm3,ftm_ch5, abs(left_motor_u));
  }
  
  if(right_motor_u >= 0)
  {
    ftm_pwm_duty(ftm3,ftm_ch6, abs(right_motor_u));
    ftm_pwm_duty(ftm3,ftm_ch7,0);
  }
  else
  {
    ftm_pwm_duty(ftm3,ftm_ch6,0);
    ftm_pwm_duty(ftm3,ftm_ch7, abs(right_motor_u));
  }
}

int16 getLeftMotor_u(void)
{
  return left_motor_u;
}

int16 getRightMotor_u(void)
{
  return right_motor_u;
}

void setLeftMotor_u(int16 value)
{
  left_motor_u = value;
}

void setRightMotor_u(int16 value)
{
  right_motor_u = value;
}

uint16 getSteer_u(void)
{
  return steer_u;
}

void setSteer_u(uint16 value)
{
  steer_u = value;
}
