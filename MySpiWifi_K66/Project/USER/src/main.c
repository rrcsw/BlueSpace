#include "headfile.h"

uint8 img_buff[IMAGE_SIZE] = { 0 };
uint8 img60_80[IMAGE_HEIGHT][IMAGE_WIDTH] = { 0 };

extern void WIFI_SPI_Init(void);

void init(void)
{
  //摄像头初始化
  camera_init();
  
  //速度FTM正交解码初始化
//  ftm_quad_init(ftm1);
//  ftm_quad_init(ftm2);
  
  //舵机驱动初始化
  ftm_pwm_init(ftm0, ftm_ch5, STEER_FREQ, STEER_MID);
  //电机驱动初始化
  ftm_pwm_init(ftm3, ftm_ch4, MOTOR_FREQ, 0);
  ftm_pwm_init(ftm3, ftm_ch5, MOTOR_FREQ, 0);
  ftm_pwm_init(ftm3, ftm_ch6, MOTOR_FREQ, 0);
  ftm_pwm_init(ftm3, ftm_ch7, MOTOR_FREQ, 0);
  
  //定时器0初始化,1ms
//  pit_init_ms(pit0,1);
//  set_irq_priority(PIT0_IRQn, 2);//设置优先级,根据自己的需求设置
//  enable_irq(PIT0_IRQn);//打开定时器pit0的中断开关
  //定时器1初始化,5ms
//  pit_init_ms(pit1,5);
//  set_irq_priority(PIT1_IRQn, 2);//设置优先级,根据自己的需求设置
//  enable_irq(PIT1_IRQn);//打开定时器pit1的中断开关
  //定时器2初始化,1ms
//  pit_init_ms(pit2,1);
//  set_irq_priority(PIT2_IRQn, 3);//设置优先级,根据自己的需求设置
//  enable_irq(PIT2_IRQn);//打开定时器pit2的中断开关
  
  //串口初始化
//  uart_init(uart0, 115200);
//  set_irq_priority(UART0_RX_TX_IRQn,3);//设置优先级,根据自己的需求设置
//  uart_rx_irq_en(uart0);//打开串口uart0的接收中断开关
  
  //WIFI_SPI初始化
  //WIFI_SPI_Init();
  
  //看门狗初始化
//  wdog_init_ms(1000);
//  wdog_enable();
  
  EnableInterrupts;
  systick_delay_ms(100);
}

extern CarInfo car_info;

void TestCamera(void)
{
  //摄像头初始化
  camera_init();
  //舵机驱动初始化
  ftm_pwm_init(ftm0, ftm_ch5, STEER_FREQ, STEER_MID);
  //电机驱动初始化
  ftm_pwm_init(ftm3, ftm_ch4, MOTOR_FREQ, 0);
  ftm_pwm_init(ftm3, ftm_ch5, MOTOR_FREQ, 0);
  ftm_pwm_init(ftm3, ftm_ch6, MOTOR_FREQ, 0);
  ftm_pwm_init(ftm3, ftm_ch7, MOTOR_FREQ, 0);
  WIFI_SPI_Init();
  
  setLeftMotor_u(4200);
  setRightMotor_u(4000);
  while(1)
  {
    if(mt9v032_finish_flag)//摄像头获取图像
    {
      mt9v032_finish_flag = 0;
      onCameraUpdated(image, img60_80, img_buff);
      static int count = 0;
      TcpData tcp_data;
      if(count >= 5)
      {
        uint16 real_len = setCarInfoPack(&tcp_data, car_info);
        PushTcpBuff(tcp_data);
      }
      else if(count >= 2)
      {
        uint16 real_len = setImagePack(&tcp_data, img_buff);
        PushTcpBuff(tcp_data);
      }
      count++;
      if(count >= 6)
      count = 0;
    
      SendTcpData();//发送消息
      
    }
  }
}

int main(void)
{
  get_clk();//上电后必须运行一次这个函数，获取各个频率信息，便于后面各个模块的参数设置
  onCreate();
  //TestCamera();
  
  init();
  onStart();
  while(1)
  {
    if(mt9v032_finish_flag)//摄像头获取图像
    {
      mt9v032_finish_flag = 0;
      onFPSUpdated();
      onCameraUpdated(image, img60_80, img_buff);
    }
    if(IsShutDown())
    {
      onStop();
      break;
    }
  }
  onDestroy();
  return 0;
}
