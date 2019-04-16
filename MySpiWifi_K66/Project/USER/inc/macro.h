#ifndef _macro_h
#define _macro_h

#include "common.h"

//-----------------------------------------/
#define DEBUG   /*  开启调试信息标志位  */
//#undef DEBUG    /*  取消调试信息标志位  */
//-----------------------------------------/

//-----------------------------------------------------------------------------------------------------
#define DIR 0 //左小右大

//-------频率---------------------
#define STEER_FREQ 100
#define MOTOR_FREQ 15000

//-------舵机pwm-------------------
#define STEER_MIN 8410
#define STEER_MID 8615
#define STEER_MAX 8820

//-------PID参数-------------------
#define STEER_P 6.8
#define STEER_I 0.0
#define STEER_D 1.2//1.4还行  //2.3//2.0

#define MOTOR_LEFT_P 0.0
#define MOTOR_LEFT_I 1.5
#define MOTOR_LEFT_D 0.0

#define MOTOR_RIGHT_P 0.0
#define MOTOR_RIGHT_I 1.5
#define MOTOR_RIGHT_D 0.0

//-------图像相关参数-----------------
#define WHITE 0xFF
#define BLACK 0x00

#define IMAGE_HEIGHT 60//图像高度
#define IMAGE_WIDTH 80//图像宽度

#define ROW 60//采集时的图像高度	范围1-480
#define COL 160//采集时的图像宽度   范围1-752     K60采集不允许超过188

#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT / 8)
#define CAMERA_FPS 100//摄像头帧数
//-------控制相关参数---------------------------------------------------------------
#define MID_LINE_VAL (IMAGE_WIDTH / 2 - 2.5)
#define INITIAL_SPEED 0

#define PWM_MAX 6000
#define BANGBANG_THRESH 80
#define BANGBANG_OUTPUT 1000

//-------其他-----------------------------------------------------------------------

#define TCPBUFF_LENGTH 640//缓冲大小
#define TCPBUFF_CAP 20//缓冲区容量

#define MSG_LENGTH 10
#define MSG_CAP 20


//-------------------------------------------------------------------------
#define CLOCK_NUM 10//超时定时器数量

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


//-------模糊算法参数----------------------------
#define FUZZY_ROW 7
#define FUZZY_COL 7
#define NB 0
#define NM 1
#define NS 2
#define ZO 3
#define PS 4
#define PM 5
#define PB 6

//--------消息池编号-----------------
#define BASE_ 0 //
#define COMM_ 1 //通信层
#define PERC_ 2 //感知层
#define DECI_ 3 //决策层
#define CTRL_ 4 //控制层

#define LAYER_SIZE 5//消息池层数

//------------------------------
#define CLOSE_LOOP 1
#define HALF_CLOSE_LOOP 2
#define OPEN_LOOP_MEETING_CAR 3
//-------------------------------------------------------------------------------------------
//-------消息ID-----------------------------------------------------------------------------
typedef enum MSG
{
  WM_NONE = 0,
  WM_COMM = 1,//通信层起始段
  
  
  WM_PERC = 21,//感知层起始段
  
  WM_DECI = 41,//决策层起始段
  
  WM_CTRL = 61,//控制层起始段
  WM_CTRL_CLOSE_LOOP_MODE,
  WM_CTRL_OPEN_LOOP_MEETING_CAR_MODE,
  
  WM_USER = 101,//保留
}MSG;

typedef struct Msg//消息体
{
  MSG id;
  uint8 data[MSG_LENGTH];
}Msg;

typedef struct msg_buffer//消息队列
{
  Msg msg[MSG_CAP];
  int head;
  int tail;
}msg_buffer;//消息缓冲器


typedef struct CarInfo
{
  char mid_line[IMAGE_HEIGHT];//平滑过的中线
  char left_line[IMAGE_HEIGHT];//原始左边线
  char right_line[IMAGE_HEIGHT];//原始右边线
  
//  uint8_t pre_mid_line[IMAGE_HEIGHT];//原始中线
//  uint8_t left_mutated_point[IMAGE_HEIGHT];//左边突变位置
//  uint8_t right_mutated_point[IMAGE_HEIGHT];//右边突变位置
//  
//  uint8_t left_patch_line[IMAGE_HEIGHT];//左边补线
//  uint8_t right_patch_line[IMAGE_HEIGHT];//右边补线
//  
//  uint8_t left_jump[IMAGE_HEIGHT];//左边跳变点的位置
//  uint8_t right_jump[IMAGE_HEIGHT];//右边跳变点的位置

  int speed_left;//左边速度传感器数值
  int speed_right;//右边速度传感器数值
//  
//  uint8_t setpoint_a;//0-80
//  int16 setpoint_v;//0-300
//  float left_pid_P;
//  float left_pid_I;
//  float right_pid_P;
//  float right_pid_I;
//  float steer_pid_P;
//  float steer_pid_D;
//  int left_motor_u;//传增量，0 + delta_u  0 - 500
//  int right_motor_u;//传增量，0 + delta_u  0 - 500
//  int steer_u;//传增量，8460 + steer_u 8340 - 8640
//  
  uint8 fps;
  uint8_t thresh;//大津法—动态二值化阈值
//  
//  uint8_t left_lose_line_num;//左边丢线数量
//  uint8_t right_lose_line_num;//右边丢线数量
//  
//  uint8_t road_type;//赛道类型
//  
//  
//  float curvity;
//  uint8_t meeting_state;//会车状态
  
  char circle_state;
  float camera_mid_line_;
  float inductor_mid_line_;
  
  float mid_line_;//拟合的中线
  char last_line;//截止行
  char longest_col;//最远点所在列
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

//------队列相关-------------------------
typedef int ElementType;

typedef struct deque_
{
    int front;//队头
    int rear;//对尾
    int size;//当前队列大小
    int capacity;//队列容量
    ElementType *data_ptr;//数据指针
}deque_;//双端循环队列

typedef deque_* deque;//双端队列指针

typedef struct TcpData//tcp消息包
{
  uint8 data[TCPBUFF_LENGTH];
}TcpData;

typedef struct queue//tcp消息队列
{
  TcpData tcp_data[TCPBUFF_CAP];
  int front;
  int rear;
}queue;//循环队列

//-------定时器相关-------------------
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

//-------会车相关---------------------
typedef enum Event
{
    EVENT_INITIAL_DETECT_MEETING_AREA = 1,//起始状态下检测会车区 事件
    
    EVENT_ANOTHER_CAR_DETECTED_MEETING_AREA,//对方先到会车区状态下检测会车区 事件
    EVENT_DETECTED_MEETING_AREA,//检测到会车区 事件
    
    EVENT_FAIL_MEETING_CAR,//会车失败 事件
    EVENT_FINISH_MEETING_CAR,//会车完成 事件

    EVENT_ANOTHER_CAR_DETECTED_FINISH_LINE,//对方先检测到终点线状态下 检测终点线 事件
    EVENT_DETECTED_FINISH_LINE,//检测终点线 事件

    EVENT_ANOTHER_CAR_FAIL,//对方失败 事件
    EVENT_OUR_CAR_FAIL,//我方失败 事件
}Event;

typedef enum State
{
    STATE_INITIAL = 1,//起始状态
    
    STATE_WAITING_FOR_MEETING,//我方先到，等待会车 状态
    STATE_ANOTHER_CAR_WAITING,//对方进入会车区 状态
    
    STATE_MEETING_CAR,//会车中 状态
    STATE_FINISH_MEETING_CAR,//我方会车完成 状态
    STATE_FAIL_MEETING_CAR,//我方会车失败 状态
    
    STATE_HAND_SHAKE,//握手 状态
    
    STATE_WAITING_FOR_DEST,//我方先接近终点 状态
    STATE_ANOTHER_CAR_WAITING_FOR_DEST,//对方先接近终点 状态
    
    STATE_RUSHING_DEST,//双方准备冲过起跑线 状态

    //STATE_ANOTHER_CAR_FAIL,//对方失败 状态
    STATE_OUR_CAR_FAIL,//我方失败 状态
    STATE_SINGLE_CAR_MODE,//单车模式 状态
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
