#include "application.h"

//uint8_t dial_switch_0_flag = 0;//���뿪��0
//uint8_t dial_switch_1_flag = 0;//���뿪��1
//uint8_t dial_switch_2_flag = 0;//���뿪��2
//uint8_t dial_switch_3_flag = 0;//���뿪��3

uint8 is_off = 0;
int16_t ref_speed = 0;//�ο��ٶ�
uint8_t fps = 0;//֡��
uint16_t fps_count = 0;

PID pid_steer;//���pid
PID pid_left_motor;//�����ٶ�pid
PID pid_right_motor;//�ҵ���ٶ�pid

CarInfo car_info;//С������״̬

float weights[IMAGE_HEIGHT] = {
                                0, 0, 0, 0, 0,                  1, 1, 1, 1, 2,
                                2, 2, 3, 4, 5,                  6, 6.5, 7, 7.5, 8,//20
                                8.5, 9.2, 9.5, 9.8, 10.5,       10.5, 10.2, 10, 9.7, 9.5,//30
                                9.2, 9, 8.7, 8.4, 8.2,          8, 7.8, 7.6, 7.4, 7.2,//40
                                7, 6.9, 6.8, 6.5, 6.2,          6, 5.8, 5.5, 5.2, 5,
                                4.8, 4.5, 4.2, 4, 3.5,          3.3, 3, 2, 1.5, 1
                               };

//ϵͳ��ʼ��ǰ
void onCreate(void)
{
  initClock();//��ʼ����ʱ��ʱ��
  initMessage();//��ʼ����Ϣ��
  initTcpBuff();//��ʼ��TCP������
  initFSM(20);//��ʼ���ᳵ״̬��
  
  //pid�ṹ���ʼ��
  initPID(&pid_steer);//��ʼ��PID
  initPID(&pid_left_motor);//��ʼ��PID
  initPID(&pid_right_motor);//��ʼ��PID
  changeSetPoint(&pid_steer, MID_LINE_VAL);//��λ λ�ã���
  changeSetPoint(&pid_left_motor, INITIAL_SPEED);//��λ cm/s
  changeSetPoint(&pid_right_motor, INITIAL_SPEED);//��λ cm/s
  setPIDParam(&pid_steer, STEER_P, STEER_I, STEER_D);//���ö��PID����
  setPIDParam(&pid_left_motor, MOTOR_LEFT_P, MOTOR_LEFT_I, MOTOR_LEFT_D);//��������PID����
  setPIDParam(&pid_right_motor, MOTOR_RIGHT_P, MOTOR_RIGHT_I, MOTOR_RIGHT_D);//�����ҵ��PID����
  
}

//ϵͳ��ʼ����
void onStart(void)
{
  setLeftMotor_u(4000);
  setRightMotor_u(3800);
}

//ϵͳ����ǰ
void onStop(void)
{
}

//ϵͳ������
void onDestroy(void)
{
}

////�������ڽ�������
//void onBluetoothReceiveData(UartEvent event)
//{
//  uint8_t cmd = event.cmd;
//  uint8_t *data = event.msg;
//  ProcBTMsg(cmd,data);
//}
//WIFI��������
void onSpiWifiReceiveData(WifiEvent event)
{
  
}

//���ٶȺ������Ǵ�����
void onAccelerationAndGyroscopeSensorUpdated(SensorEvent event)
{
  //������pid��΢����
}

//�ٶȴ�����
void onSpeedSensorUpdated(SensorEvent event)
{
//  //�ٶȲɼ����˲������ڿ�����100ms����
//  uint8_t flag = (uint8_t)event.value[0];
//  int16_t result = (int16_t)event.value[1];
//  uint16_t temp = 0;
//  if(event.sensor_type != SENSOR_SPEED)
//  {
//    car_state.speed_left_high = 0;
//    car_state.speed_left_low = 0;
//    car_state.speed_right_high = 0;
//    car_state.speed_right_low = 0;
//    return;
//  }
//  switch(flag)
//  {
//  case LEFT_ENC://��ߴ�����
//    temp = (int32_t)(FilterSpeed(v_left,result) * 200);
//    car_state.speed_left_low = (uint8_t)(temp & 0x00FF);
//    car_state.speed_left_high = (uint8_t)((temp & 0xFF00) >> 8);
//    break;
//  case RIGHT_ENC://�ұߴ�����
//    temp = (int32_t)(FilterSpeed(v_right,result) * 200);
//    car_state.speed_right_low = (uint8_t)(temp & 0x00FF);
//    car_state.speed_right_high = (uint8_t)((temp & 0xFF00) >> 8);
//    break;
//  default:
//    break;
//  }
}

//����ͷ������
/*
���ã�
  1.����ƫ�Ƽ��㣬���ڶ��pid���㣬������30-50ms��
  2.ǰ��·�����㣬���ڸı��ٶ��趨ֵ���������ȫ��
  3.����ģ�����򣬸ı�pid_v��PID����
  4.��ϱ߽��ߣ�Ҳ���ǽ�һ�г���ʶ�������ںϳɱ߽��ߣ����ڶ����Ǽ���
*/
void onCameraUpdated(uint8 img_2d[ROW][COL], uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH], uint8 img_buff[IMAGE_SIZE])
{
  //ActionLoop();//�ᳵ��ѭ��
  Perception(img_2d, img60_80, img_buff, &car_info);//��֪��
  Decision(&pid_steer, &pid_left_motor, &pid_right_motor);//���߲�
  Control(&pid_steer, &pid_left_motor, &pid_right_motor);//���Ʋ�
  //ProcessTcpData(img_buff, car_info);//ͨ�Ų�
}

//FPS����
void onFPSUpdated(void)
{
  if(fps_count <= 5)
    fps = 200;
  else
    fps = (int)(1000.0 / fps_count);
  car_info.fps = fps;
  fps_count = 0;
}

//��ʱ��1
void onTimer1Updated(void)
{
  
}

//��ʱ��2
void onTimer2Updated(void)
{
  
}

//��ʱ��3
void onTimer3Updated(void)//FPS��ʱ�ۼ�����
{
  fps_count++;
  if(fps_count > 1000)
    fps_count = 1000;
}

//��������
void onKeyUp(KeyEvent event)
{
//  uint8_t type = event.type;
//  uint8_t keycode = event.keycode;
//  ProcKUMsg(type,keycode);
}

//��������
void onKeyDown(KeyEvent event)
{
//  uint8_t type = event.type;
//  uint8_t keycode = event.keycode;
//  ProcKDMsg(type,keycode);
}

//void sendImageToBluetooth(void *imgaddr, uint32_t imgsize)
//{
//  #define CMD_IMG 0x01
//  uint8_t cmdf[1] = {REPLACED_VAL};    //����ͨ��ͷ ʹ�õ�����
//  uint8_t cmdr[1] = {REPLACED_VAL};    //����ͨ��ͷ ʹ�õ�����
////  uart_putbuff(DEBUG_PORT, cmdf, sizeof(cmdf));    //�ȷ�������
////  uart_putchar(DEBUG_PORT,CMD_IMG);    //�ȷ�������
////  uart_putbuff(DEBUG_PORT, (uint8_t *)imgaddr, imgsize); //�ٷ���ͼ��
////  uart_putbuff(DEBUG_PORT, cmdr, sizeof(cmdr));    //�ȷ�������
//}

float getLeftSpeed(void)
{
  return car_info.speed_left;
}

float getRightSpeed(void)
{
  return car_info.speed_right;
}

float getCameraMiddleLine(void)
{
  float result = 0;
  float weight_sum = 0;
  for(uint8 i = IMAGE_HEIGHT - 1; i > getLastLine(); i--)
  {
    result += weights[i] * car_info.mid_line[i];
    weight_sum += weights[i];
  }
  if(weight_sum != 0)
    result = result / weight_sum;
  else
    result = MID_LINE_VAL;
  car_info.mid_line_ = result;
  return result;
}

float getInductorMiddleLine(void)
{
  return IMAGE_WIDTH / 2;
}

float getMiddleLine(void)
{
  return getCameraMiddleLine();
}

float getAngle(void)
{
  return 0.0;
}

float getDistance(void)
{
  return 0.0;
}

uint8 getFPS(void)
{
  return 100;//car_info.fps;
}

int16_t getRefSpeed(void)
{
  return ref_speed;
}

void setRefSpeed(int16_t speed)
{
  ref_speed = speed;
}

//void turnImgFlag(void)
//{
//  bt_img_flag = (~bt_img_flag) & 0x01;
//}
//
//void turnInfoFlag(void)
//{
//  bt_info_flag = (~bt_info_flag) & 0x01;
//}
//
//void ProcBTMsg(uint8_t cmd, uint8_t data[])
//{
//  switch(cmd)
//  {
//  case 'a':
//    ProcessPrint(data);
//    break;
//  case 'b':
//    setFlag(&bt_img_flag,data);
//    break;
//  case 'c':
//    setFlag(&bt_info_flag,data);
//    break;
//  case 'd':
//    setFlag(&motor_stop_flag,data);
//    break;
//  case 'j':
//    ProcessPWMDuty(data);
//    break;
//  case 'k':
//    ProcessPWMWidth(data);
//    break;
//  case 'm':
//    ProcessSetting(&car_state,&left_pid_v,&right_pid_v,&pid_a,data);
//    break;
//  default:
//    break;
//  }
//}
//
//void ProcKDMsg(uint8_t type, uint8_t keycode)
//{
//  if(type == KEY_SHORT_PRESS)
//  {
//    switch(keycode)
//    {
//    case KEY2:
//      turnInfoFlag();
//      break;
//    case KEY3:
//      turnImgFlag();
//      break;
//    default:
//      break;
//    }
//  }
//  else if(type == KEY_LONG_PRESS)
//  {
//    switch(keycode)
//    {
//    case KEY3:
//      
//      break;
//    default:
//      break;
//    }
//  }
//}
//
//void ProcKUMsg(uint8_t type, uint8_t keycode)
//{
//  
//}
//

uint8 IsShutDown(void)
{
  return is_off;
}

void ShutDown(void)
{
  is_off = 1;
}
