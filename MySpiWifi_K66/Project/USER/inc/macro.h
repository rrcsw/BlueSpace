#ifndef _macro_h
#define _macro_h

#include "common.h"

//-----------------------------------------/
#define DEBUG   /*  ����������Ϣ��־λ  */
//#undef DEBUG    /*  ȡ��������Ϣ��־λ  */
//-----------------------------------------/

//-----------------------------------------------------------------------------------------------------
#define DIR 0 //��С�Ҵ�

//-------Ƶ��---------------------
#define STEER_FREQ 100
#define MOTOR_FREQ 15000

//-------���pwm-------------------
#define STEER_MIN 8410
#define STEER_MID 8615
#define STEER_MAX 8820

//-------PID����-------------------
#define STEER_P 6.8
#define STEER_I 0.0
#define STEER_D 1.2//1.4����  //2.3//2.0

#define MOTOR_LEFT_P 0.0
#define MOTOR_LEFT_I 1.5
#define MOTOR_LEFT_D 0.0

#define MOTOR_RIGHT_P 0.0
#define MOTOR_RIGHT_I 1.5
#define MOTOR_RIGHT_D 0.0

//-------ͼ����ز���-----------------
#define WHITE 0xFF
#define BLACK 0x00

#define IMAGE_HEIGHT 60//ͼ��߶�
#define IMAGE_WIDTH 80//ͼ����

#define ROW 60//�ɼ�ʱ��ͼ��߶�	��Χ1-480
#define COL 160//�ɼ�ʱ��ͼ����   ��Χ1-752     K60�ɼ���������188

#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT / 8)
#define CAMERA_FPS 100//����ͷ֡��
//-------������ز���---------------------------------------------------------------
#define MID_LINE_VAL (IMAGE_WIDTH / 2 - 2.5)
#define INITIAL_SPEED 0

#define PWM_MAX 6000
#define BANGBANG_THRESH 80
#define BANGBANG_OUTPUT 1000

//-------����-----------------------------------------------------------------------

#define TCPBUFF_LENGTH 640//�����С
#define TCPBUFF_CAP 20//����������

#define MSG_LENGTH 10
#define MSG_CAP 20


//-------------------------------------------------------------------------
#define CLOCK_NUM 10//��ʱ��ʱ������

//-----------------------------------------------------------------------
#define LEFT_ENC 1
#define RIGHT_ENC 2
//-------------------------------------------------------------------
#define SENSOR_SPEED 1
#define SENSOR_DISTANCE 2
#define SENSOR_INDUCTOR 3
//-------------------------------------------------
#define KEY1 1
#define KEY2 2
#define KEY3 3

#define KEY_SHORT_PRESS 1
#define KEY_LONG_PRESS  2


//-------ģ���㷨����----------------------------
#define FUZZY_ROW 7
#define FUZZY_COL 7
#define NB 0
#define NM 1
#define NS 2
#define ZO 3
#define PS 4
#define PM 5
#define PB 6

//--------��Ϣ�ر��-----------------
#define BASE_ 0 //
#define COMM_ 1 //ͨ�Ų�
#define PERC_ 2 //��֪��
#define DECI_ 3 //���߲�
#define CTRL_ 4 //���Ʋ�

#define LAYER_SIZE 5//��Ϣ�ز���

//------------------------------
#define CLOSE_LOOP 1
#define HALF_CLOSE_LOOP 2
#define OPEN_LOOP_MEETING_CAR 3
//-------------------------------------------------------------------------------------------
//-------��ϢID-----------------------------------------------------------------------------
typedef enum MSG
{
  WM_NONE = 0,
  WM_COMM = 1,//ͨ�Ų���ʼ��
  
  
  WM_PERC = 21,//��֪����ʼ��
  
  WM_DECI = 41,//���߲���ʼ��
  
  WM_CTRL = 61,//���Ʋ���ʼ��
  WM_CTRL_CLOSE_LOOP_MODE,
  WM_CTRL_OPEN_LOOP_MEETING_CAR_MODE,
  
  WM_USER = 101,//����
}MSG;

typedef struct Msg//��Ϣ��
{
  MSG id;
  uint8 data[MSG_LENGTH];
}Msg;

typedef struct msg_buffer//��Ϣ����
{
  Msg msg[MSG_CAP];
  int head;
  int tail;
}msg_buffer;//��Ϣ������


typedef struct CarInfo
{
  char mid_line[IMAGE_HEIGHT];//ƽ����������
  char left_line[IMAGE_HEIGHT];//ԭʼ�����
  char right_line[IMAGE_HEIGHT];//ԭʼ�ұ���
  
//  uint8_t pre_mid_line[IMAGE_HEIGHT];//ԭʼ����
//  uint8_t left_mutated_point[IMAGE_HEIGHT];//���ͻ��λ��
//  uint8_t right_mutated_point[IMAGE_HEIGHT];//�ұ�ͻ��λ��
//  
//  uint8_t left_patch_line[IMAGE_HEIGHT];//��߲���
//  uint8_t right_patch_line[IMAGE_HEIGHT];//�ұ߲���
//  
//  uint8_t left_jump[IMAGE_HEIGHT];//���������λ��
//  uint8_t right_jump[IMAGE_HEIGHT];//�ұ�������λ��

  int speed_left;//����ٶȴ�������ֵ
  int speed_right;//�ұ��ٶȴ�������ֵ
//  
//  uint8_t setpoint_a;//0-80
//  int16 setpoint_v;//0-300
//  float left_pid_P;
//  float left_pid_I;
//  float right_pid_P;
//  float right_pid_I;
//  float steer_pid_P;
//  float steer_pid_D;
//  int left_motor_u;//��������0 + delta_u  0 - 500
//  int right_motor_u;//��������0 + delta_u  0 - 500
//  int steer_u;//��������8460 + steer_u 8340 - 8640
//  
  uint8 fps;
  uint8_t thresh;//��򷨡���̬��ֵ����ֵ
//  
//  uint8_t left_lose_line_num;//��߶�������
//  uint8_t right_lose_line_num;//�ұ߶�������
//  
//  uint8_t road_type;//��������
//  
//  
//  float curvity;
//  uint8_t meeting_state;//�ᳵ״̬
  
  char circle_state;
  float camera_mid_line_;
  float inductor_mid_line_;
  
  float mid_line_;//��ϵ�����
  char last_line;//��ֹ��
  char longest_col;//��Զ��������
}CarInfo;

typedef struct
{
  float set_point;
  float K_p;
  float K_i;
  float K_d;
  float sum_error;
  float last_error;
  float last_2_error;
}PID;

typedef struct
{
  double X_pre;
  double P_pre;
  double Kg;
  double X_kf;
  double P_p;
}KF;

typedef struct
{
  uint8 sensor_type;
  int value[6];
}SensorEvent;

typedef struct
{
  uint8 cmd;
  uint8 msg[21];
}UartEvent;

typedef struct
{
  uint8 cmd;
  uint8 msg[50];
}WifiEvent;

typedef struct
{
  uint8 keycode;
  uint8 type;
  uint8 time;
}KeyEvent;

//------�������-------------------------
typedef int ElementType;

typedef struct deque_
{
    int front;//��ͷ
    int rear;//��β
    int size;//��ǰ���д�С
    int capacity;//��������
    ElementType *data_ptr;//����ָ��
}deque_;//˫��ѭ������

typedef deque_* deque;//˫�˶���ָ��

typedef struct TcpData//tcp��Ϣ��
{
  uint8 data[TCPBUFF_LENGTH];
}TcpData;

typedef struct queue//tcp��Ϣ����
{
  TcpData tcp_data[TCPBUFF_CAP];
  int front;
  int rear;
}queue;//ѭ������

//-------��ʱ�����-------------------
typedef struct Clock
{
  uint16 timeout;
  uint16 left_time;
  uint16 repeat_count;
  uint8 repeat_flag;
  uint8 run_flag;
  uint8 used_flag;
  void (*call_back)(void);
}Clock;

typedef struct Timer Timer;

struct Timer
{
  Clock *clock;
  //uint8 my_ptr_index;
  void (*start)(Timer *this, void (*cb_func)(void), uint16 ms);
  void (*stop)(Timer *this);
  void (*destroy)(Timer *this);
  void (*setTimeout)(Timer *this, void (*cb_func)(void), uint16 timeout);
  void (*setInterval)(Timer *this, void (*cb_func)(void), uint16 interval);
};

//-------�ᳵ���---------------------
typedef enum Event
{
    EVENT_INITIAL_DETECT_MEETING_AREA = 1,//��ʼ״̬�¼��ᳵ�� �¼�
    
    EVENT_ANOTHER_CAR_DETECTED_MEETING_AREA,//�Է��ȵ��ᳵ��״̬�¼��ᳵ�� �¼�
    EVENT_DETECTED_MEETING_AREA,//��⵽�ᳵ�� �¼�
    
    EVENT_FAIL_MEETING_CAR,//�ᳵʧ�� �¼�
    EVENT_FINISH_MEETING_CAR,//�ᳵ��� �¼�

    EVENT_ANOTHER_CAR_DETECTED_FINISH_LINE,//�Է��ȼ�⵽�յ���״̬�� ����յ��� �¼�
    EVENT_DETECTED_FINISH_LINE,//����յ��� �¼�

    EVENT_ANOTHER_CAR_FAIL,//�Է�ʧ�� �¼�
    EVENT_OUR_CAR_FAIL,//�ҷ�ʧ�� �¼�
}Event;

typedef enum State
{
    STATE_INITIAL = 1,//��ʼ״̬
    
    STATE_WAITING_FOR_MEETING,//�ҷ��ȵ����ȴ��ᳵ ״̬
    STATE_ANOTHER_CAR_WAITING,//�Է�����ᳵ�� ״̬
    
    STATE_MEETING_CAR,//�ᳵ�� ״̬
    STATE_FINISH_MEETING_CAR,//�ҷ��ᳵ��� ״̬
    STATE_FAIL_MEETING_CAR,//�ҷ��ᳵʧ�� ״̬
    
    STATE_HAND_SHAKE,//���� ״̬
    
    STATE_WAITING_FOR_DEST,//�ҷ��Ƚӽ��յ� ״̬
    STATE_ANOTHER_CAR_WAITING_FOR_DEST,//�Է��Ƚӽ��յ� ״̬
    
    STATE_RUSHING_DEST,//˫��׼����������� ״̬

    //STATE_ANOTHER_CAR_FAIL,//�Է�ʧ�� ״̬
    STATE_OUR_CAR_FAIL,//�ҷ�ʧ�� ״̬
    STATE_SINGLE_CAR_MODE,//����ģʽ ״̬
}State;

typedef enum Action
{
    action_initial_detect_meeting_area = 1,
    action_other_detect_meeting_area,
    
    action_waiting,
    
    action_meeting_car,
    action_finish_meeting_car,
    
    action_hand_shake_detect_finish_line,
    action_other_detect_finish_line,
    
    action_finish_line_waiting,
    
    action_rushing_dest,

    action_single_car_mode,
    
    action_inform_another_car_meeting_car,
    action_inform_another_car_finish_meeting_car,
    action_inform_another_car_detected_finish_line,
    action_inform_another_car_fail,
}Action;

typedef enum
{
  STOP_LINE = 1,
  STRAIGHT_LINE,
  CURVE,
  IN_CURVE,
  OUT_CURVE,
  ROUND_CIRCLE,
  CROSS_ROAD,
  OUT_BORDER,
  OTHER = 99,
  NONE = 100
}RecogResult;

#endif
